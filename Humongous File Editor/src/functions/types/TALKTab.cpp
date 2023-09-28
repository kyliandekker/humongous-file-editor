#include "functions/types/TALKTab.h"

#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/UAUDIO_WAVE_READER_RESULT.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "functions/ChunkFunctions.h"

namespace HumongousFileEditor
{
	bool TALKTab::ReplaceResource(chunk_reader::FileContainer*& fc, size_t offset)
	{
		std::string path;
		if (SoundTab::ReplaceResource(path))
		{
			if (!utils::ends_with(path, ".wav"))
				path += ".wav";

			size_t wave_size = 0;
			if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::FTell(path.c_str(), wave_size)))
				return false;

			uaudio::wave_reader::ChunkCollection chunkCollection(malloc(wave_size), wave_size);
			if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::LoadWave(path.c_str(), chunkCollection)))
				return false;

			uaudio::wave_reader::DATA_Chunk data_chunk;
			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
				return false;

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
				return false;

			if (fmt_chunk.byteRate != 11025)
				return false;

			if (fmt_chunk.sampleRate != 11025)
				return false;

			if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8)
				return false;

			if (fmt_chunk.numChannels != uaudio::wave_reader::WAVE_CHANNELS_MONO)
				return false;

			// Get TALK chunk for the raw audio data.
			size_t talk_offset = offset;

			chunk_reader::TALK_Chunk talk_chunk;
			memcpy(&talk_chunk, utils::add(fc->data, talk_offset), sizeof(chunk_reader::TALK_Chunk));

			std::vector<chunk_reader::ChunkInfo> children = fc->GetChildren(talk_offset);
			size_t hshd_offset = -1;
			size_t sdat_offset = -1;
			for (size_t i = 0; i < children.size(); i++)
			{
				if (utils::chunkcmp(children[i].chunk_id, chunk_reader::HSHD_CHUNK_ID) == 0)
					hshd_offset = children[i].offset;
				if (utils::chunkcmp(children[i].chunk_id, chunk_reader::SDAT_CHUNK_ID) == 0)
					sdat_offset = children[i].offset;
			}

			if (hshd_offset == -1)
				return false;

			if (hshd_offset == -1)
				return false;

			chunk_reader::HSHD_Chunk hshd_chunk;
			memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

			chunk_reader::SDAT_Chunk sdat_chunk;
			size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
			memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
			sdat_chunk.SetChunkSize(data_chunk.chunkSize + header_size);

			talk_chunk.SetChunkSize(
				sizeof(chunk_reader::HumongousHeader) + // DIGI chunk itself.
				hshd_chunk.ChunkSize() + // HSHD chunk.
				sdat_chunk.ChunkSize() // SDAT chunk.
			);

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(talk_chunk.ChunkSize()));
			memcpy(new_data, &talk_chunk, header_size);
			memcpy(utils::add(new_data, sizeof(talk_chunk)), &hshd_chunk, hshd_chunk.ChunkSize());
			memcpy(utils::add(new_data, sizeof(talk_chunk) + hshd_chunk.ChunkSize()), &sdat_chunk, sizeof(chunk_reader::HumongousHeader));
			memcpy(utils::add(new_data, sizeof(talk_chunk) + hshd_chunk.ChunkSize() + sizeof(chunk_reader::HumongousHeader)), data_chunk.data, data_chunk.chunkSize);

			fc->Replace(talk_offset, new_data, talk_chunk.ChunkSize());

			free(new_data);

			return true;
		}
		return false;
	}

	bool TALKTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk)
	{
		std::vector<chunk_reader::ChunkInfo> children = fc->GetChildren(offset);
		size_t hshd_offset = -1;
		size_t sdat_offset = -1;
		for (size_t i = 0; i < children.size(); i++)
		{
			if (utils::chunkcmp(children[i].chunk_id, chunk_reader::HSHD_CHUNK_ID) == 0)
				hshd_offset = children[i].offset;
			if (utils::chunkcmp(children[i].chunk_id, chunk_reader::SDAT_CHUNK_ID) == 0)
				sdat_offset = children[i].offset;
		}

		if (hshd_offset == -1)
			return false;

		if (sdat_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = utils::add(fc->data, sdat_offset + header_size);

		return true;
	}
}