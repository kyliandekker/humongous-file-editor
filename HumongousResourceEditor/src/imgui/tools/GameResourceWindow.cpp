#include "imgui/tools/GameResourceWindow.h"

#include "project/Resource.h"
#include "game/decompiled_resource/TalkResource.h"
#include "game/decompiled_resource/SongResource.h"
#include <imgui/imgui_helpers.h>

namespace resource_editor
{
	namespace imgui
	{
		GameResourceWindow::GameResourceWindow(game::GameResource& a_GameResource) : BaseTool(0, std::string(a_GameResource.m_Name + " (" + a_GameResource.m_Parent->m_Name + ")##" + a_GameResource.m_Parent->m_Path), ""), m_Resource(&a_GameResource)
		{
			switch (m_Resource->m_Type)
			{
				case game::GameResourceType::Unknown:
				{
					break;
				}
				case game::GameResourceType::Talkie:
				{
					m_Talk = new game::TalkResource(a_GameResource);
					break;
				}
				case game::GameResourceType::SFX:
				{
					break;
				}
				case game::GameResourceType::Song:
				{
					m_Song = new game::SongResource(a_GameResource);
					break;
				}
				case game::GameResourceType::RoomBackground:
				{
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

		GameResourceWindow::~GameResourceWindow()
		{
			if (m_Talk)
			{
				delete(m_Talk);
			}
			if (m_Song)
			{
				delete(m_Song);
			}
		}

		void GameResourceWindow::Render()
		{
			switch (m_Resource->m_Type)
			{
				case game::GameResourceType::Unknown:
				{
					break;
				}
				case game::GameResourceType::Talkie:
				{
					ShowValue("Sample Rate", std::string(std::to_string(m_Talk->m_HSHD_Chunk.sample_rate) + "Hz").c_str());
					ShowValue("Size", std::string(std::to_string(m_Talk->m_SDAT_Chunk.ChunkSize()) + " bytes").c_str());

					uint32_t pos = 0;
					ImGui::BeginPlayPlot(pos, m_Talk->m_SDAT_Chunk.ChunkSize(), m_Talk->m_NumSamples, m_Talk->m_Samples, "Test##D", ImGui::GetWindowSize().x, 100, 1);
					break;
				}
				case game::GameResourceType::SFX:
				{
					break;
				}
				case game::GameResourceType::Song:
				{
					ShowValue("Sample Rate", std::string(std::to_string(m_Song->m_HSHD_Chunk.sample_rate) + "Hz").c_str());
					ShowValue("Size", std::string(std::to_string(m_Song->m_SDAT_Chunk.ChunkSize()) + " bytes").c_str());

					uint32_t pos = 0;
					ImGui::BeginPlayPlot(pos, m_Song->m_SDAT_Chunk.ChunkSize(), m_Song->m_NumSamples, m_Song->m_Samples, "Test##D", ImGui::GetWindowSize().x, 100, 1);
					break;
				}
				case game::GameResourceType::RoomBackground:
				{
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