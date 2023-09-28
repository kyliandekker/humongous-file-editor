#include "functions/types/SONGTab.h"

#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "functions/ChunkFunctions.h"
#include <HumongousEditorForm.h>

namespace HumongousFileEditor
{
	bool SONGTab::ReplaceResource(chunk_reader::FileContainer*& fc, size_t offset)
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

			// Get SGEN chunk first (tells us the position of the SONG).
			chunk_reader::SGEN_Chunk sgen_chunk;
			memcpy(&sgen_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::SGEN_Chunk));

			// Get DIGI chunk for the raw audio data.
			size_t digi_offset = sgen_chunk.song_pos;

			chunk_reader::DIGI_Chunk digi_chunk;
			memcpy(&digi_chunk, utils::add(fc->data, digi_offset), sizeof(chunk_reader::DIGI_Chunk));

			std::vector<chunk_reader::ChunkInfo> children = fc->GetChildren(sgen_chunk.song_pos);
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
			sdat_chunk.SetChunkSize(data_chunk.chunkSize + static_cast<uint32_t>(header_size));

			digi_chunk.SetChunkSize(
				sizeof(chunk_reader::HumongousHeader) + // DIGI chunk itself.
				hshd_chunk.ChunkSize() + // HSHD chunk.
				sdat_chunk.ChunkSize() // SDAT chunk.
			);

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(digi_chunk.ChunkSize()));
			memcpy(new_data, &digi_chunk, header_size);
			memcpy(utils::add(new_data, sizeof(digi_chunk)), &hshd_chunk, hshd_chunk.ChunkSize());
			memcpy(utils::add(new_data, sizeof(digi_chunk) + hshd_chunk.ChunkSize()), &sdat_chunk, sizeof(chunk_reader::HumongousHeader));
			memcpy(utils::add(new_data, sizeof(digi_chunk) + hshd_chunk.ChunkSize() + sizeof(chunk_reader::HumongousHeader)), data_chunk.data, data_chunk.chunkSize);

			fc->Replace(digi_offset, new_data, digi_chunk.ChunkSize());

			uint32_t dif_size = digi_chunk.ChunkSize() - sgen_chunk.song_size;

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];

			chunk_reader::ChunkInfo next_chunk = fc->GetChunkInfo(sizeof(chunk_reader::HumongousHeader));
			while (next_chunk.offset < fc->size)
			{
				if (utils::chunkcmp(next_chunk.chunk_id, chunk_reader::SGEN_CHUNK_ID) == 0)
				{
					if (next_chunk.offset >= offset)
					{
						chunk_reader::SGEN_Chunk new_sgen_chunk;
						memcpy(&new_sgen_chunk, utils::add(fc->data, next_chunk.offset), sizeof(chunk_reader::SGEN_Chunk));

						if (next_chunk.offset == offset)
							new_sgen_chunk.song_size = digi_chunk.ChunkSize();
						else
							new_sgen_chunk.song_pos += dif_size;

						fc->Replace(next_chunk.offset, reinterpret_cast<unsigned char*>(&new_sgen_chunk), sizeof(chunk_reader::SGEN_Chunk));
					}
				}
				next_chunk = fc->GetNextChunk(next_chunk.offset);

				HumongousFileEditor::chunk_reader::ChunkFunctions::SetProgressBar(form->toolProgressBar, static_cast<double>(100.0f / fc->size * next_chunk.offset));
			}

			free(new_data);

			return true;
		}
		return false;
	}
	bool SONGTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk)
	{
		// Get SGEN chunk first (tells us the position of the SONG).
		chunk_reader::SGEN_Chunk sgen_chunk;
		memcpy(&sgen_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::SGEN_Chunk));

		std::vector<chunk_reader::ChunkInfo> children = fc->GetChildren(sgen_chunk.song_pos);
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