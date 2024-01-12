#include "game/decompiled_resource/RoomBackgroundResource.h"

#include <vector>

#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "system/audio/AudioUtils.h"
#include "imgui/ImGuiWindow.h"

namespace resource_editor
{
	namespace game
	{
		RoomBackgroundResource::RoomBackgroundResource(game::GameResource& a_Resource) : ImageResource(a_Resource)
		{
			GetData(a_Resource);
		}

		RoomBackgroundResource::RoomBackgroundResource(const RoomBackgroundResource& rhs)
		{
		}

		RoomBackgroundResource::~RoomBackgroundResource()
		{
		}

		bool RoomBackgroundResource::GetRoomBackgroundData(game::GameResource& a_Resource, ImgInfo& a_ImageInfo)
		{
			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);
			if (children.size() == 0)
			{
				return false;
			}

			size_t bsmap_offset = -1;
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

			chunk_reader::BMAP_Chunk bmap_chunk;
			size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
			memcpy(&bmap_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, bsmap_offset), header_size);
			bmap_chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, bsmap_offset + header_size);
			size_t bmap_size = bmap_chunk.ChunkSize() - header_size;

			size_t rmim_offset = a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Offset).m_Offset;
			chunk_reader::RMIM_Chunk rmim_chunk;
			memcpy(&rmim_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, rmim_offset), sizeof(chunk_reader::RMIM_Chunk));

			std::vector<chunk_reader::ChunkInfo> rmda_children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Parent->m_FileContainer.GetParent(rmim_offset).m_Offset);
			size_t apal_offset = -1;
			size_t rmhd_offset = -1;
			for (size_t i = 0; i < rmda_children.size(); i++)
			{
				if (low_level::utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::APAL_CHUNK_ID) == 0)
				{
					apal_offset = rmda_children[i].m_Offset;
				}
				if (low_level::utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::RMHD_CHUNK_ID) == 0)
				{
					rmhd_offset = rmda_children[i].m_Offset;
				}
			}

			if (rmhd_offset < 0)
			{
				return false;
			}

			chunk_reader::RMHD_Chunk rmhd_chunk;
			memcpy(&rmhd_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, rmhd_offset), sizeof(chunk_reader::RMHD_Chunk));

			if (apal_offset < 0)
			{
				return false;
			}

			chunk_reader::APAL_Chunk apal_chunk;
			header_size = sizeof(chunk_reader::APAL_Chunk);
			memcpy(&apal_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, apal_offset), header_size);
			size_t apal_size = apal_chunk.ChunkSize() - header_size;

			return GetDataBMAP(bmap_chunk, apal_chunk, bmap_chunk.data[0], rmhd_chunk.width, rmhd_chunk.height, a_ImageInfo);
		}

		bool RoomBackgroundResource::GetData(game::GameResource& a_Resource)
		{
			if (!GetRoomBackgroundData(a_Resource, m_ImageInfo))
				return false;

			imgui::window.GetDX9().CreateTexture(m_Texture, m_ImageInfo);
			return true;
		}

        bool RoomBackgroundResource::Replace(game::GameResource& a_Resource)
        {
            return false;
        }

		bool RoomBackgroundResource::Save(game::GameResource& a_Resource)
		{
			return false;
		}
	}
}