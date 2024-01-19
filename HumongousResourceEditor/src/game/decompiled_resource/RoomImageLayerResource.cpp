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

		bool RoomImageLayerResource::GetData(game::GameResource& a_Resource)
		{
			if (m_Texture)
			{
				m_ImageMutex.lock();
				m_Texture->Release();
				m_Texture = nullptr;
				m_ImageMutex.unlock();
			}

			ImgInfo roomImage;
			
			if (!RoomImageResource::GetRoomImageData(a_Resource, roomImage, m_ShowTransparency))
			{
				return false;
			}

			size_t lflf_offset = a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Parent->m_FileContainer.GetParent(a_Resource.m_Offset).m_Offset).m_Offset).m_Offset;

			std::vector<chunk_reader::ChunkInfo> lflf_children = a_Resource.m_Parent->m_FileContainer.GetChildren(lflf_offset);
			if (lflf_children.size() == 0)
			{
				return false;
			}

			std::vector<chunk_reader::ChunkInfo> desired = { chunk_reader::ChunkInfo(chunk_reader::IM00_CHUNK_ID) };
			if (low_level::utils::seekChildren(lflf_children, desired) < desired.size())
			{
				return false;
			}

			ImgInfo backgroundImage;

			game::GameResource resource = a_Resource;
			resource.m_Offset = desired[0].m_Offset;
			if (!RoomBackgroundResource::GetRoomBackgroundData(resource, backgroundImage, m_ShowTransparency))
			{
				return false;
			}

			int CHANNELS = 4;
			int cur = 0;
			for (size_t i = 0; i < backgroundImage.Size(); i += CHANNELS, cur++)
			{
				int y = cur / static_cast<int>(backgroundImage.m_Width);
				int x = cur % static_cast<int>(backgroundImage.m_Width);

				if (x >= roomImage.m_X && x < roomImage.m_X + roomImage.m_Width &&
					y >= roomImage.m_Y && y < roomImage.m_Y + roomImage.m_Height)
				{
					int relativeX = x - roomImage.m_X;
					int relativeY = y - roomImage.m_Y;

					relativeY *= roomImage.m_Width;

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
			m_ImageInfo.m_X = roomImage.m_X;
			m_ImageInfo.m_Y = roomImage.m_Y;
			imgui::window.GetDX9().CreateTexture(m_Texture, m_ImageInfo);

			a_Resource.m_Properties.emplace(std::make_pair("5. Background Width", std::string(std::to_string(backgroundImage.m_Width) + "px")));
			a_Resource.m_Properties.emplace(std::make_pair("6. Background Height", std::string(std::to_string(backgroundImage.m_Height) + "px")));
			a_Resource.m_Properties.emplace(std::make_pair("7. Image Width", std::string(std::to_string(roomImage.m_Width) + "px")));
			a_Resource.m_Properties.emplace(std::make_pair("8. Image Height", std::string(std::to_string(roomImage.m_Height) + "px")));
			a_Resource.m_Properties.emplace(std::make_pair("9. x", std::string(std::to_string(roomImage.m_X) + "px")));
			a_Resource.m_Properties.emplace(std::make_pair("10. y", std::string(std::to_string(roomImage.m_Y) + "px")));

			return true;
		}

        bool RoomImageLayerResource::Replace(game::GameResource& a_Resource)
        {
            return false;
        }

		bool RoomImageLayerResource::Save(game::GameResource& a_Resource)
		{
			return false;
		}
	}
}