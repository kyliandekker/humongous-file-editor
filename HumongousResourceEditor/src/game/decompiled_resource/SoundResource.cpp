#include "game/decompiled_resource/SoundResource.h"

#include <vector>
#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "game/GameResource.h"
#include "project/Resource.h"
#include "utils/abstractions.h"
#include "utils/string.h"
#include "system/Logger.h"
#include "allocators/ChunkCollectionWrapper.h"

namespace resource_editor
{
	namespace game
	{
		SoundResource::SoundResource(game::GameResource& a_Resource) : DecompiledResource()
		{
			GetData(a_Resource);
		}

		SoundResource::SoundResource(const SoundResource& rhs) : DecompiledResource(rhs)
		{
			m_HSHD_Chunk = rhs.m_HSHD_Chunk;
			m_SDAT_Chunk = rhs.m_SDAT_Chunk;
			m_NumSamples = rhs.m_NumSamples;
			m_Samples = reinterpret_cast<double*>(malloc(m_NumSamples * sizeof(double)));
			if (m_Samples)
			{
				memcpy(m_Samples, rhs.m_Samples, m_NumSamples * sizeof(double));
			}
		}

		SoundResource::~SoundResource()
		{
			if (m_Samples)
			{
				free(m_Samples);
			}
		}

		bool SoundResource::GetData(game::GameResource& a_Resource)
		{
			return true;
		}

		bool SoundResource::OpenResource(std::string& a_Path, uaudio::wave_reader::ChunkCollectionWrapper& a_ChunkCollection)
		{
			std::string path;
			const std::vector<COMDLG_FILTERSPEC> filters =
			{
				{L"WAVE files (*.wav)", L"*.wav;*.WAV"}
			};
			if (abstractions::PickFile(path, filters))
			{
				if (!string_extensions::ends_with(path, ".wav"))
				{
					path += ".wav";
				}

				size_t wave_size = 0;
				if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::FTell(path.c_str(), wave_size)))
				{
					LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" could not load.", path.c_str());
					return false;
				}

				a_ChunkCollection = uaudio::wave_reader::ChunkCollectionWrapper(wave_size);
				if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::LoadWave(path.c_str(), a_ChunkCollection)))
				{
					LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" could not load.", path.c_str());
					return false;
				}

				uaudio::wave_reader::FMT_Chunk fmt_chunk;
				uaudio::wave_reader::DATA_Chunk data_chunk;

				if (UAUDIOWAVEREADERFAILED(a_ChunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
				{
					LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" is missing a data chunk.", path.c_str());
					return false;
				}

				if (UAUDIOWAVEREADERFAILED(a_ChunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
				{
					LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" is missing a fmt chunk.", path.c_str());
					return false;
				}

				if (fmt_chunk.byteRate != uaudio::wave_reader::WAVE_SAMPLE_RATE_11025)
				{
					LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" needs to have a byterate of 11025hz.", path.c_str());
					return false;
				}

				if (fmt_chunk.sampleRate != uaudio::wave_reader::WAVE_SAMPLE_RATE_11025)
				{
					LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" needs to have a samplerate of 11025hz.", path.c_str());
					return false;
				}

				if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8)
				{
					LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" needs to have a bps of 8.", path.c_str());
					return false;
				}

				if (fmt_chunk.numChannels != uaudio::wave_reader::WAVE_CHANNELS_MONO)
				{
					LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" needs to be mono.", path.c_str());
					return false;
				}

				a_Path = path;

				return true;
			}

			return false;
		}

		bool SoundResource::SaveResource(std::string& a_Path)
		{
			std::string path;
			const std::vector<COMDLG_FILTERSPEC> filters =
			{
				{L"WAVE files (*.wav)", L"*.wav;*.WAV"}
			};
			if (abstractions::SaveFile(path))
			{
				if (!string_extensions::ends_with(path, ".wav"))
				{
					path += ".wav";
				}
				a_Path = path;
				return true;
			}
			return false;
		}

        bool SoundResource::SaveSound(std::string a_Path, void* a_Data, size_t a_DataSize, uint16_t a_SampleRate)
        {
			FILE* file = nullptr;
			fopen_s(&file, a_Path.c_str(), "wb");
			if (file == nullptr)
			{
				LOG(logger::LOGSEVERITY_ERROR, "Could not export resource.");
				return false;
			}

			uaudio::wave_reader::FMT_Chunk fmt_chunk(uaudio::wave_reader::FMT_CHUNK_ID);
			fmt_chunk.audioFormat = uaudio::wave_reader::WAV_FORMAT_UNCOMPRESSED;
			fmt_chunk.bitsPerSample = uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8;
			fmt_chunk.blockAlign = uaudio::wave_reader::BLOCK_ALIGN_8_BIT_MONO;
			fmt_chunk.sampleRate = a_SampleRate;
			fmt_chunk.numChannels = uaudio::wave_reader::WAVE_CHANNELS_MONO;
			fmt_chunk.byteRate = a_SampleRate;
			fmt_chunk.SetChunkSize(sizeof(uaudio::wave_reader::FMT_Chunk) - sizeof(uaudio::wave_reader::ChunkHeader));

			uaudio::wave_reader::DATA_Chunk data_chunk(uaudio::wave_reader::DATA_CHUNK_ID);
			data_chunk.SetChunkSize(static_cast<uint32_t>(a_DataSize));
			data_chunk.data = reinterpret_cast<unsigned char*>(a_Data);

			uaudio::wave_reader::RIFF_Chunk riff_chunk(uaudio::wave_reader::RIFF_CHUNK_ID);
			riff_chunk.SetChunkSize(
				sizeof(uaudio::wave_reader::FMT_Chunk) +
				(sizeof(uaudio::wave_reader::RIFF_Chunk) - sizeof(uaudio::wave_reader::ChunkHeader)) +
				sizeof(uaudio::wave_reader::DATA_Chunk) - sizeof(data_chunk.data) +
				static_cast<uint32_t>(a_DataSize));
			memcpy(riff_chunk.format, uaudio::wave_reader::RIFF_CHUNK_FORMAT, uaudio::wave_reader::CHUNK_ID_SIZE);

			fwrite(&riff_chunk, sizeof(riff_chunk), 1, file);
			fwrite(&fmt_chunk, sizeof(fmt_chunk), 1, file);
			fwrite(&data_chunk, sizeof(uaudio::wave_reader::ChunkHeader), 1, file);
			fwrite(a_Data, a_DataSize, 1, file);

			fclose(file);

			return true;
        }
	}
}