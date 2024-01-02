#include "imgui/tools/GameResourceWindow.h"

#include <imgui/imgui_helpers.h>

#include "project/Resource.h"
#include "game/decompiled_resource/TalkResource.h"
#include "game/decompiled_resource/SongResource.h"
#include "game/decompiled_resource/SFXResource.h"
#include "game/decompiled_resource/RoomBackgroundResource.h"
#include <imgui/ImGuiWindow.h>

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

			ShowValue("Name:", m_Resource->m_Name.c_str());
			ShowValue("Path:", std::string(m_Resource->m_Parent->m_Name + " (" + m_Resource->m_Parent->m_Path + ")").c_str());

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

					ShowValue("Sample Rate:", std::string(std::to_string(sound->m_HSHD_Chunk.sample_rate) + "Hz").c_str());
					ShowValue("Size:", std::string(std::to_string(sound->m_SDAT_Chunk.ChunkSize()) + " bytes").c_str());

					uint32_t pos = 0;
					ImGui::BeginPlayPlot(pos, sound->m_SDAT_Chunk.ChunkSize(), sound->m_NumSamples, sound->m_Samples, "Test##D", ImGui::GetWindowSize().x, 100, 1);
					break;
				}
				case game::GameResourceType::RoomBackground:
				{
					game::ImageResource* image = dynamic_cast<game::ImageResource*>(m_ResourceData);

					ImGui::Image((void*)image->m_Texture, ImVec2(image->m_ImageInfo.m_Width, image->m_ImageInfo.m_Height));
					break;
				}
				case game::GameResourceType::RoomImage:
				{
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