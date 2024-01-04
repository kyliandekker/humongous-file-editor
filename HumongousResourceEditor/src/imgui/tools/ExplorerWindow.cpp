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

		void ExplorerTool::RenderResource(project::Resource& a_Resource, bool& a_ShowPopUp)
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
						name = ICON_FA_FOLDER_OPEN + std::string(" ") + name;
					else
						name = ICON_FA_FOLDER + std::string(" ") + name;

					ImGui::SetNextItemOpen(a_Resource.m_FoldedOut);
					bool fold = ImGui::TreeNodeS(name.c_str(), id.c_str());
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
						if (ImGui::IsItemHovered() && ImGui::IsItemClicked(1))
						{
							a_ShowPopUp |= true;
							m_SelectedResource = &a_Resource;
						}
						for (size_t i = 0; i < a_Resource.m_Resources.size(); i++)
						{
							RenderResource(a_Resource.m_Resources[i], a_ShowPopUp);
						}
						ImGui::TreePop();
					}
				}
				else if (!a_Resource.m_HasChildren)
				{
					name = ICON_FA_FOLDER + std::string(" ") + name;
					if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf))
					{
						if (ImGui::IsItemHovered() && ImGui::IsItemClicked(1))
						{
							a_ShowPopUp |= true;
							m_SelectedResource = &a_Resource;
						}
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
				if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::IsItemHovered() && ImGui::IsItemClicked(1))
					{
						a_ShowPopUp |= true;
						m_SelectedResource = &a_Resource;
					}
					ImGui::TreePop();
				}
			}

			if (a_ShowPopUp)
			{
				ImGui::OpenPopup("res_popup");
			}
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
					if (m_SelectedResource->m_FileContainer.m_Size == 0)
					{
						if (ImGui::MenuItem("Load"))
						{
							// Clear existing project file first.
							if (m_LoadedResources[(int)m_SelectedResource->m_ResourceType])
							{
								project::Resource* resource = m_LoadedResources[(int)m_SelectedResource->m_ResourceType];
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
								m_LoadedResources[(int)m_SelectedResource->m_ResourceType] = nullptr;
							}
							m_LoadedResources[(int)m_SelectedResource->m_ResourceType] = m_SelectedResource;
							if (m_SelectedResource->m_FileContainer.Open(m_SelectedResource->m_Path))
							{
								switch (m_SelectedResource->m_ResourceType)
								{
									case project::ResourceType::HE0:
									case project::ResourceType::A:
									{
										project::Resource* a = nullptr;
										for (size_t i = 0; i < m_SelectedResource->m_Parent->m_Resources.size(); i++)
										{
											project::Resource& resource = m_SelectedResource->m_Parent->m_Resources[i];
											if (string_extensions::getFileWithoutExtension(resource.m_Name).compare(string_extensions::getFileWithoutExtension(m_SelectedResource->m_Name)) == 0 && resource.m_ResourceType == project::ResourceType::A)
											{
												a = &resource;
												break;
											}
										}
										if (a == nullptr)
										{
											return;
										}

										m_LoadedResources[(int)project::ResourceType::A] = a;
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

								resourcesWindow.SetActiveTab((int)m_SelectedResource->m_ResourceType);
							}
						}
					}
					else if (m_LoadedResources[(int)m_SelectedResource->m_ResourceType] == m_SelectedResource)
					{
						if (ImGui::MenuItem("Unload"))
						{
							project::Resource* resource = m_LoadedResources[(int)m_SelectedResource->m_ResourceType];
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
							m_LoadedResources[(int)m_SelectedResource->m_ResourceType] = nullptr;
						}
					}
				}
				ImGui::EndPopup();
			}
		}
	}
}