#include "game/decompiled_resource/SoundResource.h"

#include <vector>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "game/GameResource.h"
#include "project/Resource.h"
#include "utils/abstractions.h"
#include "utils/string.h"

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
			if (m_SDAT_Chunk.data)
			{
				free(m_SDAT_Chunk.data);
			}
		}

		bool SoundResource::GetData(game::GameResource& a_Resource)
		{
			return true;
		}

		bool SoundResource::ReplaceResource(game::GameResource& a_Resource)
		{
			return true;
		}

		// TODO: Check for memory leaks.
		bool SoundResource::OpenResource(std::string& a_Path, uaudio::wave_reader::ChunkCollection a_ChunkCollection)
		{
			std::string path;
			std::vector<COMDLG_FILTERSPEC> filters =
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
					return false;
				}

				a_ChunkCollection = uaudio::wave_reader::ChunkCollection(malloc(wave_size), wave_size);
				if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::LoadWave(path.c_str(), a_ChunkCollection)))
				{
					free(a_ChunkCollection.data());
					return false;
				}

				uaudio::wave_reader::FMT_Chunk fmt_chunk;
				uaudio::wave_reader::DATA_Chunk data_chunk;

				if (UAUDIOWAVEREADERFAILED(a_ChunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
				{
					free(a_ChunkCollection.data());
					return false;
				}

				if (UAUDIOWAVEREADERFAILED(a_ChunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
				{
					free(a_ChunkCollection.data());
					return false;
				}

				if (fmt_chunk.byteRate != 11025)
				{
					free(a_ChunkCollection.data());
					return false;
				}

				if (fmt_chunk.sampleRate != 11025)
				{
					free(a_ChunkCollection.data());
					return false;
				}

				if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8)
				{
					free(a_ChunkCollection.data());
					return false;
				}

				if (fmt_chunk.numChannels != uaudio::wave_reader::WAVE_CHANNELS_MONO)
				{
					free(a_ChunkCollection.data());
					return false;
				}

				a_Path = path;

				return true;
			}

			return false;
		}
	}
}