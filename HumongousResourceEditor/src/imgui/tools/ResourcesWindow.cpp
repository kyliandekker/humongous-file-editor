#include "imgui/tools/ResourcesWindow.h"

#include "game/compilers/SongFileCompiler.h"
#include "game/compilers/ResourceFileCompiler.h"
#include "game/compilers/TalkFileCompiler.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "imgui/ImguiDefines.h"

namespace resource_editor
{
	namespace imgui
	{
		ResourcesWindow::ResourcesWindow(std::string a_Name, project::Resource& a_Resource) : BaseTool(0, a_Name, ""), m_Resource(&a_Resource)
		{ }

		void renderGameResource(game::GameResource & resource, game::GameResource*& selectedResource, bool& showPopUp)
		{
			std::string name = resource.m_Name;
			std::string id = "##" + resource.m_Name;

			switch (resource.m_Type)
			{
				case game::GameResourceType::Unknown:
				{
					name = ICON_FA_FILE_CIRCLE_QUESTION + std::string(" ") + name + "(UNKNOWN)";
					break;
				}
				case game::GameResourceType::Talkie:
				{
					name = ICON_FA_VOLUME_UP + std::string(" ") + name + "(TALK)";
					break;
				}
				case game::GameResourceType::SFX:
				{
					name = ICON_FA_VOLUME_UP + std::string(" ") + name + "(SFX)";
					break;
				}
				case game::GameResourceType::Song:
				{
					name = ICON_FA_MUSIC + std::string(" ") + name + "(SONG)";
					break;
				}
				case game::GameResourceType::RoomBackground:
				{
					name = ICON_FA_FILE_IMAGE_O + std::string(" ") + name + (resource.m_Special ? "" : "(BACKGROUND)");
					break;
				}
				case game::GameResourceType::RoomImage:
				{
					name = ICON_FA_FILE_IMAGE_O + std::string(" ") + name + (resource.m_Special ? "" : "(IMAGE)");
					break;
				}
				case game::GameResourceType::Local_Script:
				{
					name = ICON_FA_FILE_TEXT + std::string(" ") + name + "(LOCAL SCRIPT)";
					break;
				}
				case game::GameResourceType::Global_Script:
				{
					name = ICON_FA_FILE_TEXT + std::string(" ") + name + "(GLOBAL SCRIPT)";
					break;
				}
				case game::GameResourceType::Verb_Script:
				{
					name = ICON_FA_FILE_TEXT + std::string(" ") + name + "(VERB SCRIPT)";
					break;
				}
				case game::GameResourceType::Room:
				{
					break;
				}
			}

			name += id;
			if (ImGui::TreeNodeEx(name.c_str(), resource.m_Resources.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None))
			{
				for (size_t i = 0; i < resource.m_Resources.size(); i++)
				{
					renderGameResource(resource.m_Resources[i], selectedResource, showPopUp);
				}
				ImGui::TreePop();
			}
			if (ImGui::IsItemHovered() && ImGui::IsItemClicked(1))
			{
				showPopUp |= true;
				selectedResource = &resource;
			}

			if (showPopUp)
				ImGui::OpenPopup("gameres_popup");
		}

		void ResourcesWindow::Render()
		{
			bool showPopUp = false;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 10.0f });
			for (size_t i = 0; i < m_Resource->m_GameResources.size(); i++)
			{
				renderGameResource(m_Resource->m_GameResources[i], m_SelectedResource, showPopUp);
			}
			ImGui::PopStyleVar();
			if (m_SelectedResource)
			{
				if (ImGui::BeginPopup("gameres_popup"))
				{
					if (ImGui::MenuItem("Export"))
					{

					}
					else
					{
						std::string window_id = m_SelectedResource->m_Name + "##resource_window_" + m_SelectedResource->m_Name + m_SelectedResource->m_Parent->m_Path;
						if (!m_SelectedResource->m_OpenedWindow && ImGui::MenuItem("Show Window"))
						{
							bool found = false;
							for (size_t i = 0; i < m_Windows.size(); i++)
							{
								if (m_Windows[i].GetName() == window_id)
								{
									found = true;
								}
							}
							if (!found)
							{
								m_SelectedResource->m_OpenedWindow = true;
								m_Windows.emplace_back((*m_SelectedResource));
							}
						}
						else if (m_SelectedResource->m_OpenedWindow && ImGui::MenuItem("Hide Window"))
						{
							for (size_t i = 0; i < m_Windows.size(); i++)
							{
								if (m_Windows[i].GetName() == window_id)
								{
									m_Windows.erase(m_Windows.begin() + i);
								}
								m_SelectedResource->m_OpenedWindow = false;
							}
						}
					}
					ImGui::EndPopup();
				}
			}
			for (size_t i = 0; i < m_Windows.size(); i++)
			{
				m_Windows[i].Update();
			}
		}
	}
}