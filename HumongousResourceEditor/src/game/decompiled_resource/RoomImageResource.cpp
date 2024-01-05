#include "game/decompiled_resource/RoomImageResource.h"

#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "system/audio/AudioUtils.h"

#include "low_level/HumongousChunks.h"

#include <vector>

namespace resource_editor
{
	namespace game
	{
		RoomImageResource::RoomImageResource(game::GameResource& a_Resource) : ImageResource(a_Resource)
		{
			GetData(a_Resource);
		}

		RoomImageResource::RoomImageResource(const RoomImageResource& rhs) : ImageResource(rhs)
		{
		}

		RoomImageResource::~RoomImageResource()
		{
		}

		bool RoomImageResource::GetData(game::GameResource& a_Resource)
		{
			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);
			if (children.size() == 0)
				return false;

			size_t obim_offset = a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Offset).m_Offset;

			int32_t bsmap_offset = -1;
			for (size_t i = 0; i < children.size(); i++)
			{
				if (low_level::utils::chunkcmp(children[i].chunk_id, chunk_reader::BMAP_CHUNK_ID) == 0 || low_level::utils::chunkcmp(children[i].chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
				{
					bsmap_offset = children[i].m_Offset;
				}
			}

			if (bsmap_offset < 0)
			{
				return false;
			}

			std::vector<chunk_reader::ChunkInfo> obim_children = a_Resource.m_Parent->m_FileContainer.GetChildren(obim_offset);
			int32_t imhd_offset = -1;
			for (size_t i = 0; i < obim_children.size(); i++)
			{
				if (low_level::utils::chunkcmp(obim_children[i].chunk_id, chunk_reader::IMHD_CHUNK_ID) == 0)
				{
					imhd_offset = static_cast<int32_t>(obim_children[i].m_Offset);
				}
			}

			if (imhd_offset < 0)
			{
				return false;
			}

			chunk_reader::IMHD_Chunk imhd_chunk;
			memcpy(&imhd_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, imhd_offset), sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data));
			imhd_chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, imhd_offset + (sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data)));

			std::vector<chunk_reader::ChunkInfo> rmda_children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Parent->m_FileContainer.GetParent(obim_offset).m_Offset);
			int32_t apal_offset = -1;
			for (size_t i = 0; i < rmda_children.size(); i++)
			{
				if (low_level::utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::APAL_CHUNK_ID) == 0)
				{
					apal_offset = static_cast<int32_t>(rmda_children[i].m_Offset);
				}
			}

			if (apal_offset < 0)
			{
				return false;
			}

			chunk_reader::APAL_Chunk apal_chunk;
			size_t header_size = sizeof(chunk_reader::APAL_Chunk);
			memcpy(&apal_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, apal_offset), header_size);

			chunk_reader::ChunkInfo bsmap = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(bsmap_offset);

			if (low_level::utils::chunkcmp(bsmap.chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
			{
				chunk_reader::SMAP_Chunk smap_chunk;
				size_t smap_header_size = sizeof(chunk_reader::SMAP_Chunk) - sizeof(smap_chunk.data); // Pointer in the SMAP class is size 8 and needs to be deducted.
				memcpy(&smap_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, bsmap_offset), smap_header_size);
				smap_chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, bsmap_offset + smap_header_size);

				return GetDataSMAP(smap_chunk, apal_chunk, imhd_chunk.width, imhd_chunk.height);
			}
			else
			{
				chunk_reader::BMAP_Chunk bmap_chunk;
				size_t bmap_header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
				memcpy(&bmap_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, bsmap_offset), bmap_header_size);
				bmap_chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, bsmap_offset + bmap_header_size);

				return GetDataBMAP(bmap_chunk, apal_chunk, bmap_chunk.fill_color, imhd_chunk.width, imhd_chunk.height);
			}
			return false;
		}
	}
}