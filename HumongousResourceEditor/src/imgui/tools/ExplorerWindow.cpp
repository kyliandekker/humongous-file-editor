#include "imgui/tools/ExplorerWindow.h"

#include "project/Project.h"
#include "imgui/ImguiDefines.h"
#include "imgui/tools/ResourcesWindow.h"

#include "game/compilers/SongFileCompiler.h"
#include "game/compilers/TalkFileCompiler.h"
#include "game/compilers/ResourceFileCompiler.h"

namespace resource_editor
{
	namespace imgui
	{
		ExplorerTool::ExplorerTool() : BaseTool(ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse, "Explorer", "Explorer")
		{ }

		void ExplorerTool::RenderResource(project::Resource& resource, bool& showPopUp)
		{
			if (!resource.m_Show)
			{
				return;
			}

			std::string name = resource.m_Name;
			std::string id = "##" + resource.m_Path;

			if (resource.m_ResourceType == project::ResourceType::Folder)
			{
				if (resource.m_HasChildren)
				{
					if (resource.m_FoldedOut)
						name = ICON_FA_FOLDER_OPEN + std::string(" ") + name;
					else
						name = ICON_FA_FOLDER + std::string(" ") + name;

					bool fold = ImGui::TreeNodeS(name.c_str(), id.c_str());
					if (resource.m_FoldedOut != fold)
					{
						resource.m_FoldedOut = fold;
						if (fold)
						{
							resource.Scan();
						}
					}
					if (resource.m_FoldedOut)
					{
						if (ImGui::IsItemHovered() && ImGui::IsItemClicked(1))
						{
							showPopUp |= true;
							m_SelectedResource = &resource;
						}
						for (size_t i = 0; i < resource.m_Resources.size(); i++)
						{
							RenderResource(resource.m_Resources[i], showPopUp);
						}
						ImGui::TreePop();
					}
				}
				else if (!resource.m_HasChildren)
				{
					name = ICON_FA_FOLDER + std::string(" ") + name;
					if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf))
					{
						if (ImGui::IsItemHovered() && ImGui::IsItemClicked(1))
						{
							showPopUp |= true;
							m_SelectedResource = &resource;
						}
						ImGui::TreePop();
					}
				}
			}
			else
			{
				if (resource.m_FileContainer.size == 0)
				{
					name = ICON_FA_FILE_CIRCLE_QUESTION + std::string(" ") + name;
				}
				else
				{
					name = ICON_FA_FILE + std::string(" ") + name;
				}
				if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::IsItemHovered() && ImGui::IsItemClicked(1))
					{
						showPopUp |= true;
						m_SelectedResource = &resource;
					}
					ImGui::TreePop();
				}
			}

			if (showPopUp)
				ImGui::OpenPopup("res_popup");
		}

		void ExplorerTool::Render()
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 10.0f });
			bool showPopUp = false;
			RenderResource(project::project.m_Folder, showPopUp);
			ImGui::PopStyleVar();
			if (ImGui::BeginPopup("res_popup"))
			{
				if (m_SelectedResource->m_ResourceType == project::ResourceType::Folder)
				{
					if (ImGui::MenuItem("Set As Output Folder"))
					{
					}
				}
				else
				{
					if (m_SelectedResource->m_FileContainer.size == 0)
					{
						if (ImGui::MenuItem("Load"))
						{
							if (m_SelectedResource->m_FileContainer.Open(m_SelectedResource->m_Path))
							{
								switch (m_SelectedResource->m_ResourceType)
								{
									case project::ResourceType::HE0:
									case project::ResourceType::A:
									{
										game::ResourceFileCompiler compiler;
										compiler.Decompile(*m_SelectedResource, m_SelectedResource->m_GameResources);
										break;
									}
									case project::ResourceType::HE2:
									{
										game::TalkFileCompiler compiler;
										compiler.Decompile(*m_SelectedResource, m_SelectedResource->m_GameResources);
										break;
									}
									case project::ResourceType::HE4:
									{
										game::SongFileCompiler compiler;
										compiler.Decompile(*m_SelectedResource, m_SelectedResource->m_GameResources);
										break;
									}
								}
							}
						}
					}
					else
					{
						if (ImGui::MenuItem("Unload"))
						{
							if (m_SelectedResource->m_FileContainer.Unload())
							{
								// TODO: Pop up message.
							}
						}
						else
						{
							std::string window_id = m_SelectedResource->m_Name + "##" + m_SelectedResource->m_Path;
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
									m_Windows.push_back(ResourcesWindow(window_id, *m_SelectedResource));
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
								}
								m_SelectedResource->m_OpenedWindow = false;
							}
						}
					}
				}
				ImGui::EndPopup();
			}
			for (size_t i = 0; i < m_Windows.size(); i++)
			{
				m_Windows[i].Update();
			}
		}
	}
}