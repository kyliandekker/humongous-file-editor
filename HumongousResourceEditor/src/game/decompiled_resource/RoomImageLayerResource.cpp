#include "game/decompiled_resource/RoomImageLayerResource.h"

#include <vector>

#include "low_level/HumongousChunks.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "low_level/HumongousChunkDefinitions.h"
#include "game/decompiled_resource/RoomImageResource.h"
#include "game/decompiled_resource/RoomBackgroundResource.h"
#include "imgui/ImGuiWindow.h"

namespace resource_editor
{
	namespace game
	{
		RoomImageLayerResource::RoomImageLayerResource(game::GameResource& a_Resource) : ImageResource(a_Resource)
		{
			GetData(a_Resource);
		}

		RoomImageLayerResource::RoomImageLayerResource(const RoomImageLayerResource& rhs) : ImageResource(rhs)
		{
		}

		RoomImageLayerResource::~RoomImageLayerResource()
		{
		}

		bool RoomImageLayerResource::GetData(game::GameResource& a_Resource)
		{
			ImgInfo roomImage;
			
			if (!RoomImageResource::GetRoomImageData(a_Resource, roomImage))
			{
				return false;
			}

			size_t lflf_offset = a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Offset).m_Offset).m_Offset).m_Offset;
			std::vector<chunk_reader::ChunkInfo> lflf_children = a_Resource.m_Parent->m_FileContainer.GetChildren(lflf_offset);
			size_t im00_offset = -1;
			for (size_t i = 0; i < lflf_children.size(); i++)
			{
				if (low_level::utils::chunkcmp(lflf_children[i].chunk_id, chunk_reader::IM00_CHUNK_ID) == 0)
				{
					im00_offset = lflf_children[i].m_Offset;
					break;
				}
			}

			if (im00_offset < 0)
			{
				return false;
			}

			ImgInfo backgroundImage;

			game::GameResource resource = a_Resource;
			resource.m_Offset = im00_offset;
			if (!RoomBackgroundResource::GetRoomBackgroundData(resource, backgroundImage))
			{
				return false;
			}

			size_t obim_offset = a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Offset).m_Offset;
			std::vector<chunk_reader::ChunkInfo> obim_children = a_Resource.m_Parent->m_FileContainer.GetChildren(obim_offset);
			size_t imhd_offset = -1;
			for (size_t i = 0; i < obim_children.size(); i++)
			{
				if (low_level::utils::chunkcmp(obim_children[i].chunk_id, chunk_reader::IMHD_CHUNK_ID) == 0)
				{
					imhd_offset = obim_children[i].m_Offset;
				}
			}

			if (imhd_offset < 0)
			{
				return false;
			}

			chunk_reader::IMHD_Chunk imhd_chunk;
			memcpy(&imhd_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, imhd_offset), sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data));
			imhd_chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, imhd_offset + (sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data)));

			int CHANNELS = 4;
			int cur = 0;
			for (size_t i = 0; i < backgroundImage.Size(); i += CHANNELS, cur++)
			{
				int y = cur / static_cast<int>(backgroundImage.m_Width);
				int x = cur % static_cast<int>(backgroundImage.m_Width);

				if (x >= imhd_chunk.x && x < imhd_chunk.x + imhd_chunk.width &&
					y >= imhd_chunk.y && y < imhd_chunk.y + imhd_chunk.height)
				{
					int relativeX = x - imhd_chunk.x;
					int relativeY = y - imhd_chunk.y;

					relativeY *= imhd_chunk.width;

					int index = relativeX + relativeY;

					index *= 4;

					if (roomImage.m_Data[index + 3] != 0)
					{
						backgroundImage.m_Data[i] = roomImage.m_Data[index];
						backgroundImage.m_Data[i + 1] = roomImage.m_Data[index + 1];
						backgroundImage.m_Data[i + 2] = roomImage.m_Data[index + 2];
						backgroundImage.m_Data[i + 3] = roomImage.m_Data[index + 3];
					}
				}
				else
				{
					backgroundImage.m_Data[i + 3] = 55;
				}
			}

			m_ImageInfo = backgroundImage;
			m_ImageInfo.m_Width2 = roomImage.m_Width;
			m_ImageInfo.m_Height2 = roomImage.m_Height;
			m_ImageInfo.m_X = imhd_chunk.x;
			m_ImageInfo.m_Y = imhd_chunk.y;
			imgui::window.GetDX9().CreateTexture(m_Texture, m_ImageInfo);

			return true;
		}
	}
}