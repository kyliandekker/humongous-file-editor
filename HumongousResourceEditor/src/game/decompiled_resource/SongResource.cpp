#include "game/decompiled_resource/SongResource.h"

#include <vector>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>

#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "system/audio/AudioUtils.h"
#include "system/Logger.h"

namespace resource_editor
{
	namespace game
	{
		SongResource::SongResource(game::GameResource& a_Resource) : SoundResource(a_Resource)
		{
			GetData(a_Resource);
		}

		SongResource::SongResource(const SongResource& rhs) : SoundResource(rhs)
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

		bool SongResource::GetData(game::GameResource& a_Resource)
		{
			// Get SGEN chunk first (tells us the position of the SONG).
			chunk_reader::SGEN_Chunk sgen_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(sgen_chunk, a_Resource.m_Offset);

			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(sgen_chunk.song_pos);
			if (children.size() == 0)
			{
				return false;
			}

			std::vector<chunk_reader::ChunkInfo> desired = {
				chunk_reader::ChunkInfo(chunk_reader::HSHD_CHUNK_ID),
				chunk_reader::ChunkInfo(chunk_reader::SDAT_CHUNK_ID),
			};

			if (low_level::utils::seekChildren(children, desired) < desired.size())
			{
				return false;
			}

			a_Resource.m_Parent->m_FileContainer.GetChunk(m_HSHD_Chunk, desired[0].m_Offset);

			a_Resource.m_Parent->m_FileContainer.GetChunk(m_SDAT_Chunk, desired[1].m_Offset, sizeof(chunk_reader::HumongousHeader));
			m_SDAT_Chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[1].m_Offset + sizeof(chunk_reader::HumongousHeader));

			m_NumSamples = m_SDAT_Chunk.ChunkSize();
			m_Samples = audio::utils::ToSample(m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize());

			a_Resource.m_Properties.emplace(std::make_pair("4. Sample Rate", std::string(std::to_string(m_HSHD_Chunk.sample_rate) + "Hz")));
			a_Resource.m_Properties.emplace(std::make_pair("5. Channels", std::to_string(1) + " (mono)"));
			a_Resource.m_Properties.emplace(std::make_pair("6. Bits Per Sample", std::to_string(8)));
			a_Resource.m_Properties.emplace(std::make_pair("7. Size", std::string(std::to_string(m_SDAT_Chunk.ChunkSize()) + " bytes")));

			return true;
		}

		bool SongResource::Replace(game::GameResource& a_Resource)
		{
			std::string path;
			uaudio::wave_reader::ChunkCollection chunkCollection;
			if (OpenResource(path, chunkCollection))
			{
				//uaudio::wave_reader::FMT_Chunk fmt_chunk;
				//uaudio::wave_reader::DATA_Chunk data_chunk;

				//if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
				//{
				//	LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" does not have a data chunk.", path.c_str());
				//	free(chunkCollection.data());
				//	return false;
				//}

				//if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
				//{
				//	LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" does not have a fmt chunk.", path.c_str());
				//	free(chunkCollection.data());
				//	return false;
				//}

				//// Get SGEN chunk first (tells us the position of the SONG).
				//chunk_reader::SGEN_Chunk sgen_chunk;
				//memcpy(&sgen_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, a_Resource.m_Offset), sizeof(chunk_reader::SGEN_Chunk));

				//// Get DIGI chunk for the raw audio data.
				//size_t digi_offset = sgen_chunk.song_pos;

				//chunk_reader::DIGI_Chunk digi_chunk;
				//memcpy(&digi_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, digi_offset), sizeof(chunk_reader::DIGI_Chunk));

				//std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(sgen_chunk.song_pos);
				//if (children.size() == 0)
				//{
				//	free(chunkCollection.data());
				//	return false;
				//}

				//std::vector<chunk_reader::ChunkInfo> desired = { chunk_reader::ChunkInfo(chunk_reader::HSHD_CHUNK_ID), chunk_reader::ChunkInfo(chunk_reader::SDAT_CHUNK_ID) };
				//if (low_level::utils::seekChildren(children, desired) < desired.size())
				//{
				//	free(chunkCollection.data());
				//	return false;
				//}

				//chunk_reader::HSHD_Chunk hshd_chunk;
				//memcpy(&hshd_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[0].m_Offset), sizeof(chunk_reader::HSHD_Chunk));

				//chunk_reader::SDAT_Chunk sdat_chunk;
				//size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
				//memcpy(&sdat_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[1].m_Offset), header_size);
				//sdat_chunk.SetChunkSize(data_chunk.chunkSize + static_cast<uint32_t>(header_size));

				//digi_chunk.SetChunkSize(
				//	sizeof(chunk_reader::HumongousHeader) + // DIGI chunk itself.
				//	hshd_chunk.ChunkSize() + // HSHD chunk.
				//	sdat_chunk.ChunkSize() // SDAT chunk.
				//);

				//unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(digi_chunk.ChunkSize()));
				//if (!new_data)
				//{
				//	LOGF(logger::LOGSEVERITY_ERROR, "Could not allocate memory when trying to replace resource with file \"%s\".", path.c_str());
				//	free(chunkCollection.data());
				//	return false;
				//}
				//memcpy(new_data, &digi_chunk, sizeof(chunk_reader::SDAT_Chunk));
				//memcpy(low_level::utils::add(new_data, sizeof(digi_chunk)), &hshd_chunk, hshd_chunk.ChunkSize());
				//memcpy(low_level::utils::add(new_data, sizeof(digi_chunk) + hshd_chunk.ChunkSize()), &sdat_chunk, sizeof(chunk_reader::HumongousHeader));
				//memcpy(low_level::utils::add(new_data, sizeof(digi_chunk) + hshd_chunk.ChunkSize() + sizeof(chunk_reader::HumongousHeader)), data_chunk.data, data_chunk.chunkSize);

				//a_Resource.m_Parent->m_FileContainer.Replace(digi_offset, new_data, digi_chunk.ChunkSize());

				//int32_t dif_size = digi_chunk.ChunkSize() - sgen_chunk.song_size;

				//chunk_reader::ChunkInfo next_chunk = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(sizeof(chunk_reader::HumongousHeader));
				//while (next_chunk.m_Offset < a_Resource.m_Parent->m_FileContainer.m_Size)
				//{
				//	if (low_level::utils::chunkcmp(next_chunk.chunk_id, chunk_reader::SGEN_CHUNK_ID) == 0)
				//	{
				//		if (next_chunk.m_Offset >= a_Resource.m_Offset)
				//		{
				//			chunk_reader::SGEN_Chunk new_sgen_chunk;
				//			memcpy(&new_sgen_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, next_chunk.m_Offset), sizeof(chunk_reader::SGEN_Chunk));

				//			if (next_chunk.m_Offset == a_Resource.m_Offset)
				//			{
				//				new_sgen_chunk.song_size = digi_chunk.ChunkSize();
				//			}
				//			else
				//			{
				//				new_sgen_chunk.song_pos += dif_size;
				//			}

				//			a_Resource.m_Parent->m_FileContainer.Replace(next_chunk.m_Offset, reinterpret_cast<unsigned char*>(&new_sgen_chunk), sizeof(chunk_reader::SGEN_Chunk));
				//		}
				//	}
				//	next_chunk = a_Resource.m_Parent->m_FileContainer.GetNextChunk(next_chunk.m_Offset);
				//}

				//free(new_data);
				//free(chunkCollection.data());

				return true;
			}
			return false;
		}

		bool SongResource::Save(game::GameResource& a_Resource)
		{
			std::string path;
			if (SaveResource(path))
			{
				const bool saved = SaveSound(path, m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize() - sizeof(resource_editor::chunk_reader::SDAT_Chunk), m_HSHD_Chunk.sample_rate);
				if (saved)
				{
					system(path.c_str());
				}
				return saved;
			}
			return false;
		}
	}
}