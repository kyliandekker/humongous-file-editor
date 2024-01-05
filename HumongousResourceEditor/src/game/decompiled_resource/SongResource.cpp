#include "game/decompiled_resource/SongResource.h"

#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "system/audio/AudioUtils.h"

#include <vector>

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

		SongResource::~SongResource()
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

		bool SongResource::GetData(game::GameResource& a_Resource)
		{
			// Get SGEN chunk first (tells us the position of the SONG).
			chunk_reader::SGEN_Chunk sgen_chunk;
			memcpy(&sgen_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, a_Resource.m_Offset), sizeof(chunk_reader::SGEN_Chunk));

			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(sgen_chunk.song_pos);
			size_t hshd_offset = -1;
			size_t sdat_offset = -1;
			for (size_t i = 0; i < children.size(); i++)
			{
				if (low_level::utils::chunkcmp(children[i].chunk_id, chunk_reader::HSHD_CHUNK_ID) == 0)
				{
					hshd_offset = children[i].m_Offset;
				}
				if (low_level::utils::chunkcmp(children[i].chunk_id, chunk_reader::SDAT_CHUNK_ID) == 0)
				{
					sdat_offset = children[i].m_Offset;
				}
			}

			if (hshd_offset == -1)
			{
				return false;
			}

			if (sdat_offset == -1)
			{
				return false;
			}

			memcpy(&m_HSHD_Chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

			size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(m_SDAT_Chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
			memcpy(&m_SDAT_Chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, sdat_offset), header_size);
			m_SDAT_Chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, sdat_offset + header_size);

			m_NumSamples = m_SDAT_Chunk.ChunkSize();
			m_Samples = audio::utils::ToSample(m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize());

			return true;
		}
	}
}