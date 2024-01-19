#include "imgui/tools/ExplorerWindow.h"

#include "project/Project.h"
#include "imgui/ImguiDefines.h"
#include "imgui/tools/ResourcesWindow.h"

#include "game/compilers/SongFileCompiler.h"
#include "game/compilers/TalkFileCompiler.h"
#include "game/compilers/ResourceFileCompiler.h"
#include "utils/string.h"

resource_editor::imgui::ExplorerTool resource_editor::imgui::explorerWindow;

namespace resource_editor
{
	namespace imgui
	{
		ExplorerTool::ExplorerTool() : BaseTool(0, "Explorer")
		{ }

		void ExplorerTool::RenderResource(project::Resource& a_Resource)
		{
			if (a_Resource.m_Name.empty())
			{
				return;
			}

			if (!a_Resource.m_Show)
			{
				return;
			}

			std::string name = a_Resource.m_Name;
			std::string id = "##" + a_Resource.m_Path;

			if (a_Resource.m_ResourceType == project::ResourceType::Folder)
			{
				if (a_Resource.m_HasChildren)
				{
					if (a_Resource.m_FoldedOut)
					{
						name = ICON_FA_FOLDER_OPEN + std::string(" ") + name;
					}
					else
					{
						name = ICON_FA_FOLDER + std::string(" ") + name;
					}

					ImGui::SetNextItemOpen(a_Resource.m_FoldedOut);
					bool fold = ImGui::TreeNodeS(name.c_str(), id.c_str());

					if (ImGui::IsItemHovered() && ImGui::IsItemClicked(1))
					{
						m_ShowPopUp |= true;
						m_SelectedResource = &a_Resource;
					}

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
					{
						m_DoubleClick |= true;
						m_SelectedResource = &a_Resource;
					}

					if (a_Resource.m_FoldedOut != fold)
					{
						a_Resource.m_FoldedOut = fold;
						if (fold)
						{
							a_Resource.Scan();
						}
					}
					if (a_Resource.m_FoldedOut)
					{
						for (auto& resource : a_Resource.m_Resources)
						{
							RenderResource(resource);
						}
						ImGui::TreePop();
					}
				}
				else if (!a_Resource.m_HasChildren)
				{
					name = ICON_FA_FOLDER + std::string(" ") + name;
					bool opened = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf);

					if (ImGui::IsItemHovered() && ImGui::IsItemClicked(1))
					{
						m_ShowPopUp |= true;
						m_SelectedResource = &a_Resource;
					}

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
					{
						m_DoubleClick |= true;
						m_SelectedResource = &a_Resource;
					}

					if (opened)
					{
						ImGui::TreePop();
					}
				}
			}
			else
			{
				if (a_Resource.m_FileContainer.m_Size == 0)
				{
					name = ICON_FA_FILE_CIRCLE_QUESTION + std::string(" ") + name;
				}
				else
				{
					name = ICON_FA_FILE + std::string(" ") + name;
				}
				bool opened = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf);

				if (ImGui::IsItemHovered() && ImGui::IsItemClicked(1))
				{
					m_ShowPopUp |= true;
					m_SelectedResource = &a_Resource;
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					m_DoubleClick |= true;
					m_SelectedResource = &a_Resource;
				}

				if (opened)
				{
					ImGui::TreePop();
				}
			}

			if (m_ShowPopUp)
			{
				ImGui::OpenPopup("res_popup");
			}
		}

		void ExplorerTool::Render()
		{
			m_ShowPopUp = false;
			m_DoubleClick = false;

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 10.0f });
			RenderResource(project::project.m_Folder);
			ImGui::PopStyleVar();
			if (m_SelectedResource)
			{
				if (m_DoubleClick)
				{
					project::project.UnloadResource(m_SelectedResource->m_ResourceType);
					project::project.LoadResourceWithPath(*m_SelectedResource);

					resourcesWindow.SetActiveTab((int)m_SelectedResource->m_ResourceType);
				}
				else if (ImGui::BeginPopup("res_popup"))
				{
					if (m_SelectedResource->m_ResourceType != project::ResourceType::Folder)
					{
						if (m_SelectedResource->m_FileContainer.m_Size == 0)
						{
							if (ImGui::MenuItem("Load"))
							{
								project::project.UnloadResource(m_SelectedResource->m_ResourceType);
								project::project.LoadResourceWithPath(*m_SelectedResource);

								resourcesWindow.SetActiveTab((int)m_SelectedResource->m_ResourceType);
							}
						}
						else if (project::project.m_LoadedResources[(int)m_SelectedResource->m_ResourceType] == m_SelectedResource)
						{
							if (ImGui::MenuItem("Unload"))
							{
								project::project.UnloadResource(m_SelectedResource->m_ResourceType);
							}
						}
					}
					ImGui::EndPopup();
				}
			}
		}
	}
}