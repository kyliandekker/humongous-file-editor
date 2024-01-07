#include "imgui/tools/GameResourceWindow.h"

#include <imgui/imgui_helpers.h>

#include "project/Resource.h"
#include "game/decompiled_resource/TalkResource.h"
#include "game/decompiled_resource/SongResource.h"
#include "game/decompiled_resource/SFXResource.h"
#include "game/decompiled_resource/RoomBackgroundResource.h"
#include "game/decompiled_resource/RoomImageResource.h"
#include "game/decompiled_resource/RoomImageLayerResource.h"
#include "imgui/ImGuiWindow.h"
#include "imgui/ImGuiDefines.h"
#include "system/AudioSystem.h"

resource_editor::imgui::GameResourceWindow resource_editor::imgui::gameResourceWindow;

namespace resource_editor
{
	namespace imgui
	{
		GameResourceWindow::GameResourceWindow() : BaseTool(0, std::string("Game Resource Window"))
		{ }

		void GameResourceWindow::Render()
		{
			if (!m_Resource)
				return;

			for (auto& pair : m_Resource->m_Properties)
			{
				ShowValue(pair.first, pair.second.c_str());
			}
			

			if (!m_ResourceData)
				return;

			switch (m_Resource->m_Type)
			{
				case game::GameResourceType::Unknown:
				{
					break;
				}
				case game::GameResourceType::Talkie:
				case game::GameResourceType::Song:
				case game::GameResourceType::SFX:
				{
					game::SoundResource* sound = dynamic_cast<game::SoundResource*>(m_ResourceData);

					ShowValue("Sample Rate", std::string(std::to_string(sound->m_HSHD_Chunk.sample_rate) + "Hz").c_str());
					ShowValue("Size", std::string(std::to_string(sound->m_SDAT_Chunk.ChunkSize()) + " bytes").c_str());

					ImGui::BeginPlayPlot(sound->m_NumSamples, sound->m_Samples, "Test##D", ImGui::GetWindowSize().x, 100, 1);

					if (ImGui::Button(std::string(PLAY + std::string(" Play")).c_str()))
					{
						audioSystem.Stop();
						audioSystem.Play(sound->m_SDAT_Chunk.data, sound->m_SDAT_Chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader) - sizeof(sound->m_SDAT_Chunk.data));
					}
					else if (ImGui::Button(std::string(STOP + std::string(" Stop")).c_str()))
					{
						audioSystem.Stop();
					}
					break;
				}
				case game::GameResourceType::RoomImage:
				case game::GameResourceType::RoomBackground:
				case game::GameResourceType::RoomImageLayer:
				{
					game::ImageResource* image = dynamic_cast<game::ImageResource*>(m_ResourceData);
					ShowValue("Size", std::string(std::to_string(image->m_ImageInfo.Size()) + " bytes").c_str());

					if (m_Resource->m_Type != game::GameResourceType::RoomImageLayer)
					{
						ShowValue("Width", std::to_string(image->m_ImageInfo.m_Width).c_str());
						ShowValue("Height", std::to_string(image->m_ImageInfo.m_Height).c_str());
					}

					if (m_Resource->m_Type != game::GameResourceType::RoomBackground)
					{
						ShowValue("X", std::to_string(image->m_ImageInfo.m_X).c_str());
						ShowValue("Y", std::to_string(image->m_ImageInfo.m_Y).c_str());

						if (m_Resource->m_Type == game::GameResourceType::RoomImageLayer)
						{
							ShowValue("Image Width", std::to_string(image->m_ImageInfo.m_Width2).c_str());
							ShowValue("Image Height", std::to_string(image->m_ImageInfo.m_Height2).c_str());

							ShowValue("Background Width", std::to_string(image->m_ImageInfo.m_Width).c_str());
							ShowValue("Background Height", std::to_string(image->m_ImageInfo.m_Height).c_str());
						}
					}

					int width_new = ImGui::GetWindowSize().x;
					int height_new = (int) ((float)image->m_ImageInfo.m_Height * (1.0f / image->m_ImageInfo.m_Width * width_new));
					ImGui::Image((void*)image->m_Texture, ImVec2(width_new, height_new));
					break;
				}
				case game::GameResourceType::Local_Script:
				{
					break;
				}
				case game::GameResourceType::Global_Script:
				{
					break;
				}
				case game::GameResourceType::Verb_Script:
				{
					break;
				}
				case game::GameResourceType::Room:
				{
					break;
				}
				case game::GameResourceType::Category:
				{
					break;
				}
			}
		}

		void GameResourceWindow::SetResource(game::GameResource& a_Resource)
		{
			if (m_ResourceData)
				delete(m_ResourceData);

			m_ResourceData = nullptr;

			m_Resource = &a_Resource;
			switch (m_Resource->m_Type)
			{
				case game::GameResourceType::Unknown:
				{
					break;
				}
				case game::GameResourceType::Talkie:
				{
					m_ResourceData = new game::TalkResource(*m_Resource);
					break;
				}
				case game::GameResourceType::SFX:
				{
					m_ResourceData = new game::SFXResource(*m_Resource);
					break;
				}
				case game::GameResourceType::Song:
				{
					m_ResourceData = new game::SongResource(*m_Resource);
					break;
				}
				case game::GameResourceType::RoomBackground:
				{
					m_ResourceData = new game::RoomBackgroundResource(*m_Resource);
					break;
				}
				case game::GameResourceType::RoomImage:
				{
					m_ResourceData = new game::RoomImageResource(*m_Resource);
					break;
				}
				case game::GameResourceType::RoomImageLayer:
				{
					m_ResourceData = new game::RoomImageLayerResource(*m_Resource);
					break;
				}
				case game::GameResourceType::Local_Script:
				{
					break;
				}
				case game::GameResourceType::Global_Script:
				{
					break;
				}
				case game::GameResourceType::Verb_Script:
				{
					break;
				}
				case game::GameResourceType::Room:
				{
					break;
				}
				case game::GameResourceType::Category:
				{
					break;
				}
			}
		}
	}
}