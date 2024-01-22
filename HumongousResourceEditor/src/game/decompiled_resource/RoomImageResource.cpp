#include "game/decompiled_resource/RoomImageResource.h"

#include <vector>
#include <algorithm>

#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "system/audio/AudioUtils.h"
#include "low_level/HumongousChunks.h"
#include "imgui/ImGuiWindow.h"

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

		bool RoomImageResource::GetRoomImageData(game::GameResource& a_Resource, ImgInfo& a_ImageInfo, bool a_ShowTransparency)
		{
			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);
			if (children.size() == 0)
			{
				return false;
			}

			std::vector<chunk_reader::ChunkInfo> desired = { chunk_reader::ChunkInfo(chunk_reader::BMAP_CHUNK_ID), chunk_reader::ChunkInfo(chunk_reader::SMAP_CHUNK_ID) };
			if (low_level::utils::seekChildren(children, desired) < desired.size() / 2)
			{
				return false;
			}

			const size_t obim_offset = a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Offset).m_Offset;

			std::vector<chunk_reader::ChunkInfo> obim_children = a_Resource.m_Parent->m_FileContainer.GetChildren(obim_offset);
			if (obim_children.size() == 0)
			{
				return false;
			}

			const size_t bsmap_offset = max(desired[0].m_Offset, desired[1].m_Offset);

			desired = { chunk_reader::ChunkInfo(chunk_reader::IMHD_CHUNK_ID) };
			if (low_level::utils::seekChildren(obim_children, desired) < desired.size())
			{
				return false;
			}

			chunk_reader::IMHD_Chunk imhd_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(imhd_chunk, desired[0].m_Offset, sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data));
			imhd_chunk.data = reinterpret_cast<unsigned char*>(low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[0].m_Offset + (sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data))));

			a_ImageInfo.m_X = imhd_chunk.x;
			a_ImageInfo.m_Y = imhd_chunk.y;

			std::vector<chunk_reader::ChunkInfo> rmda_children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Parent->m_FileContainer.GetParent(obim_offset).m_Offset);
			if (rmda_children.size() == 0)
			{
				return false;
			}

			desired = { chunk_reader::ChunkInfo(chunk_reader::APAL_CHUNK_ID) };
			if (low_level::utils::seekChildren(rmda_children, desired) < desired.size())
			{
				return false;
			}

			chunk_reader::APAL_Chunk apal_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(apal_chunk, desired[0].m_Offset, sizeof(chunk_reader::APAL_Chunk));

			chunk_reader::ChunkInfo bsmap = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(bsmap_offset);

			if (low_level::utils::chunkcmp(bsmap.chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
			{
				chunk_reader::SMAP_Chunk smap_chunk;
				a_Resource.m_Parent->m_FileContainer.GetChunk(smap_chunk, bsmap_offset, sizeof(chunk_reader::SMAP_Chunk) - sizeof(smap_chunk.data));
				smap_chunk.data = reinterpret_cast<unsigned char*>(low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, bsmap_offset + sizeof(chunk_reader::HumongousHeader)));
				return GetDataSMAP(smap_chunk, apal_chunk, imhd_chunk.width, imhd_chunk.height, a_ImageInfo, a_ShowTransparency);
			}
			else
			{
				chunk_reader::BMAP_Chunk bmap_chunk;
				a_Resource.m_Parent->m_FileContainer.GetChunk(bmap_chunk, bsmap_offset, sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data));
				bmap_chunk.data = reinterpret_cast<unsigned char*>(low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, bsmap_offset + sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data)));
				return GetDataBMAP(bmap_chunk, apal_chunk, bmap_chunk.fill_color, imhd_chunk.width, imhd_chunk.height, a_ImageInfo, a_ShowTransparency);
			}
			return false;
		}

		bool RoomImageResource::GetData(game::GameResource& a_Resource)
		{
			if (m_Texture)
			{
				m_ImageMutex.lock();
				m_Texture->Release();
				m_Texture = nullptr;
				m_ImageMutex.unlock();
			}

			if (!GetRoomImageData(a_Resource, m_ImageInfo, m_ShowTransparency))
				return false;

			a_Resource.m_Properties.emplace(std::make_pair("4. Size", std::string(std::to_string(m_ImageInfo.Size()) + " bytes")));
			a_Resource.m_Properties.emplace(std::make_pair("5. Width", std::string(std::to_string(m_ImageInfo.m_Width) + "px")));
			a_Resource.m_Properties.emplace(std::make_pair("6. Height", std::string(std::to_string(m_ImageInfo.m_Height) + "px")));
			a_Resource.m_Properties.emplace(std::make_pair("7. x", std::string(std::to_string(m_ImageInfo.m_X) + "px")));
			a_Resource.m_Properties.emplace(std::make_pair("8. y", std::string(std::to_string(m_ImageInfo.m_Y) + "px")));

			imgui::window.GetDX9().CreateTexture(m_Texture, m_ImageInfo);
			return true;
		}

        bool RoomImageResource::Replace(game::GameResource& a_Resource)
        {
            return false;
        }

		bool RoomImageResource::Save(game::GameResource& a_Resource)
		{
			return false;
		}
	}
}