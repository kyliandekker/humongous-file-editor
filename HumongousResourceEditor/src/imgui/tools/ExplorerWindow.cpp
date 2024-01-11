#include "imgui/tools/ExplorerWindow.h"

#include "project/Project.h"
#include "imgui/ImguiDefines.h"
#include "imgui/tools/ResourcesWindow.h"

#include "game/compilers/SongFileCompiler.h"
#include "game/compilers/TalkFileCompiler.h"
#include "game/compilers/ResourceFileCompiler.h"
#include "utils/string.h"

resource_editor::imgui::ExplorerTool resource_editor::imgui::explorer;

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
						for (size_t i = 0; i < a_Resource.m_Resources.size(); i++)
						{
							RenderResource(a_Resource.m_Resources[i]);
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

		void ExplorerTool::LoadResource(project::Resource& a_Resource)
		{
			UnloadResource(a_Resource.m_ResourceType);

			m_LoadedResources[(int)a_Resource.m_ResourceType] = m_SelectedResource;
			if (a_Resource.m_FileContainer.Open(a_Resource.m_Path))
			{
				switch (a_Resource.m_ResourceType)
				{
					case project::ResourceType::HE0:
					case project::ResourceType::A:
					{
						project::Resource* a = nullptr;
						for (size_t i = 0; i < a_Resource.m_Parent->m_Resources.size(); i++)
						{
							project::Resource& resource = a_Resource.m_Parent->m_Resources[i];
							if (string_extensions::getFileWithoutExtension(resource.m_Name).compare(string_extensions::getFileWithoutExtension(a_Resource.m_Name)) == 0 && resource.m_ResourceType == project::ResourceType::A)
							{
								a = &resource;
								break;
							}
						}
						if (!a)
						{
							return;
						}

						m_LoadedResources[(int)project::ResourceType::A] = a;
						game::ResourceFileCompiler compiler;
						compiler.Decompile(*m_SelectedResource, a_Resource.m_GameResources);
						break;
					}
					case project::ResourceType::HE2:
					{
						game::TalkFileCompiler compiler;
						compiler.Decompile(*m_SelectedResource, a_Resource.m_GameResources);
						break;
					}
					case project::ResourceType::HE4:
					{
						game::SongFileCompiler compiler;
						compiler.Decompile(*m_SelectedResource, a_Resource.m_GameResources);
						break;
					}
				}

				resourcesWindow.SetActiveTab((int)a_Resource.m_ResourceType);
			}
		}

		void ExplorerTool::UnloadResource(project::ResourceType a_ResourceType)
		{
			project::Resource* resource = m_LoadedResources[(int)a_ResourceType];
			if (!resource)
			{
				return;
			}

			resource->m_FileContainer.Unload();
			resource->m_GameResources.clear();
			if (resource->m_ResourceType == project::ResourceType::HE0)
			{
				if (m_LoadedResources[(int)project::ResourceType::A])
				{
					project::Resource* resource = m_LoadedResources[(int)project::ResourceType::A];
					resource->m_FileContainer.Unload();
					resource->m_GameResources.clear();
					m_LoadedResources[(int)project::ResourceType::A] = nullptr;
				}
			}
			m_LoadedResources[(int)a_ResourceType] = nullptr;
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
					LoadResource(*m_SelectedResource);
				}
				else if (ImGui::BeginPopup("res_popup"))
				{
					if (m_SelectedResource->m_ResourceType != project::ResourceType::Folder)
					{
						if (m_SelectedResource->m_FileContainer.m_Size == 0)
						{
							if (ImGui::MenuItem("Load"))
							{
								LoadResource(*m_SelectedResource);
							}
						}
						else if (m_LoadedResources[(int)m_SelectedResource->m_ResourceType] == m_SelectedResource)
						{
							if (ImGui::MenuItem("Unload"))
							{
								UnloadResource(m_SelectedResource->m_ResourceType);
							}
						}
					}
					ImGui::EndPopup();
				}
			}
		}
	}
}