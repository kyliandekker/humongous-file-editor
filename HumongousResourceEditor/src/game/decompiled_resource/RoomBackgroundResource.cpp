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
		{ }

		bool RoomBackgroundResource::GetRoomBackgroundData(game::GameResource& a_Resource, ImgInfo& a_ImageInfo, bool a_ShowTransparency)
		{
			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);
			if (children.size() == 0)
			{
				return false;
			}

			std::vector<chunk_reader::ChunkInfo> desired = { chunk_reader::ChunkInfo(chunk_reader::BMAP_CHUNK_ID) };
			if (low_level::utils::seekChildren(children, desired) < desired.size())
			{
				return false;
			}

			chunk_reader::BMAP_Chunk bmap_chunk;
			const size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data);
			a_Resource.m_Parent->m_FileContainer.GetChunk(bmap_chunk, desired[0].m_Offset, sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data));
			bmap_chunk.data = reinterpret_cast<unsigned char*>(low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[0].m_Offset + header_size));

			const size_t rmim_offset = a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Offset).m_Offset;
			chunk_reader::RMIM_Chunk rmim_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(rmim_chunk, rmim_offset);

			std::vector<chunk_reader::ChunkInfo> rmda_children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Parent->m_FileContainer.GetParent(rmim_offset).m_Offset);
			if (rmda_children.size() == 0)
			{
				return false;
			}

			desired = { chunk_reader::ChunkInfo(chunk_reader::RMHD_CHUNK_ID), chunk_reader::ChunkInfo(chunk_reader::APAL_CHUNK_ID), chunk_reader::ChunkInfo(chunk_reader::TRNS_CHUNK_ID) };
			if (low_level::utils::seekChildren(rmda_children, desired) < desired.size())
			{
				return false;
			}

			chunk_reader::RMHD_Chunk rmhd_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(rmhd_chunk, desired[0].m_Offset);

			chunk_reader::APAL_Chunk apal_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(apal_chunk, desired[1].m_Offset);

			chunk_reader::TRNS_Chunk trns_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(trns_chunk, desired[2].m_Offset);

			return GetDataBMAP(bmap_chunk, apal_chunk, bmap_chunk.fill_color, rmhd_chunk.width, rmhd_chunk.height, a_ImageInfo, a_ShowTransparency);
		}

		bool RoomBackgroundResource::GetData(game::GameResource& a_Resource)
		{
			if (m_Texture)
			{
				m_ImageMutex.lock();
				m_Texture->Release();
				m_Texture = nullptr;
				m_ImageMutex.unlock();
			}

			if (!GetRoomBackgroundData(a_Resource, m_ImageInfo, m_ShowTransparency))
			{
				return false;
			}

			a_Resource.m_Properties.emplace(std::make_pair("4. Size", std::string(std::to_string(m_ImageInfo.Size()) + " bytes")));
			a_Resource.m_Properties.emplace(std::make_pair("5. Width", std::string(std::to_string(m_ImageInfo.m_Width) + "px")));
			a_Resource.m_Properties.emplace(std::make_pair("6. Height", std::string(std::to_string(m_ImageInfo.m_Height) + "px")));

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