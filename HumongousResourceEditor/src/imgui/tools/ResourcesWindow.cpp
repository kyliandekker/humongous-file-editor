#include "imgui/tools/ResourcesWindow.h"

#include <array>
#include <string>

#include "game/compilers/SongFileCompiler.h"
#include "game/compilers/ResourceFileCompiler.h"
#include "game/compilers/TalkFileCompiler.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "imgui/ImguiDefines.h"
#include "imgui/tools/ExplorerWindow.h"
#include "imgui/tools/GameResourceWindow.h"

resource_editor::imgui::ResourcesWindow resource_editor::imgui::resourcesWindow;

namespace resource_editor
{
	namespace imgui
	{
		ResourcesWindow::ResourcesWindow() : BaseTool(0, "Resource Window")
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

		std::array<std::string, 6> names
		{
			"",
			"HE0",
			"HE2",
			"HE4",
			"",
			"",
		};

		void ResourcesWindow::Render()
		{
			if (explorer.m_LoadedResources[(int)project::ResourceType::HE0] != nullptr)
			{
				if (ImGui::Button("HE0", ImVec2(100.0f, 0.0f)))
				{
					SetActiveTab((int)project::ResourceType::HE0);
				}
			}
			if (explorer.m_LoadedResources[(int)project::ResourceType::HE2] != nullptr)
			{
				ImGui::SameLine(0.0, 2.0f);
				if (ImGui::Button("HE2", ImVec2(100.0f, 0.0f)))
				{
					SetActiveTab((int)project::ResourceType::HE2);
				}
			}
			if (explorer.m_LoadedResources[(int)project::ResourceType::HE4] != nullptr)
			{
				ImGui::SameLine(0.0, 2.0f);
				if (ImGui::Button("HE4", ImVec2(100.0f, 0.0f)))
				{
					SetActiveTab((int)project::ResourceType::HE4);
				}
			}

			if (m_ActiveTab == 0 || m_ActiveTab > (int)project::ResourceType::HE4)
				return;

			project::Resource* m_Resource = explorer.m_LoadedResources[m_ActiveTab];

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
					if (ImGui::MenuItem("View"))
					{
						gameResourceWindow.SetResource(*m_SelectedResource);
					}
					else if (ImGui::MenuItem("Export"))
					{

					}
					ImGui::EndPopup();
				}
			}
		}

		void ResourcesWindow::SetActiveTab(int a_Selected)
		{
			m_ActiveTab = a_Selected;
		}
	}
}