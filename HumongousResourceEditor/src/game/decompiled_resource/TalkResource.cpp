#include "game/decompiled_resource/TalkResource.h"

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
		constexpr int MAX_SAMPLES = 70560;

		TalkResource::TalkResource(game::GameResource& a_Resource)
		{
			GetData(a_Resource);
		}

		bool TalkResource::GetData(game::GameResource& a_Resource)
		{
			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);
			size_t hshd_offset = -1;
			size_t sdat_offset = -1;
			for (size_t i = 0; i < children.size(); i++)
			{
				if (low_level::utils::chunkcmp(children[i].chunk_id, chunk_reader::HSHD_CHUNK_ID) == 0)
				{
					hshd_offset = children[i].offset;
				}
				if (low_level::utils::chunkcmp(children[i].chunk_id, chunk_reader::SDAT_CHUNK_ID) == 0)
				{
					sdat_offset = children[i].offset;
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

			memcpy(&m_HSHD_Chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

			size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(m_SDAT_Chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
			memcpy(&m_SDAT_Chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.data, sdat_offset), header_size);
			m_SDAT_Chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.data, sdat_offset + header_size);

			m_NumSamples = m_SDAT_Chunk.ChunkSize() / 4;
			m_Samples = audio::utils::ToSample(m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize(), m_NumSamples);

			return true;
		}
	}
}