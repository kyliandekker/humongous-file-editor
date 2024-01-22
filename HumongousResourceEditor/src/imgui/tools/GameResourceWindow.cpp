#include "imgui/tools/GameResourceWindow.h"

#include <imgui/imgui_helpers.h>

#include "project/Resource.h"
#include "game/decompiled_resource/TalkResource.h"
#include "game/decompiled_resource/SongResource.h"
#include "game/decompiled_resource/SFXResource.h"
#include "game/decompiled_resource/RoomBackgroundResource.h"
#include "game/decompiled_resource/RoomImageResource.h"
#include "game/decompiled_resource/RoomImageLayerResource.h"
#include "game/decompiled_resource/ScriptResource.h"
#include "imgui/ImGuiWindow.h"
#include "imgui/ImGuiDefines.h"
#include "system/AudioSystem.h"
#include "project/Project.h"
#include "system/Logger.h"

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
			{
				return;
			}

			for (auto& pair : m_Resource->m_Properties)
			{
				ShowValue(pair.first, pair.second.c_str());
			}

			if (!m_ResourceData)
			{
				return;
			}

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

					ImGui::BeginPlayPlot(sound->m_NumSamples, sound->m_Samples, "Test##D", ImGui::GetWindowSize().x, 100, 1);

					if (ImGui::Button(std::string(ICON_PLAY + std::string(" Play")).c_str(), ImVec2(75, 25)))
					{
						audioSystem.Stop();
						audioSystem.Play(sound->m_SDAT_Chunk.data, sound->m_SDAT_Chunk.DataSize() - sizeof(sound->m_SDAT_Chunk.data));
					}
					ImGui::SameLine();
					if (ImGui::Button(std::string(ICON_STOP + std::string(" Stop")).c_str(), ImVec2(75, 25)))
					{
						audioSystem.Stop();
					}
					ImGui::SameLine();
					break;
				}
				case game::GameResourceType::RoomImage:
				case game::GameResourceType::RoomBackground:
				case game::GameResourceType::RoomImageLayer:
				{
					game::ImageResource* image = dynamic_cast<game::ImageResource*>(m_ResourceData);

					const float width_new = ImGui::GetWindowSize().x;
					const float height_new = image->m_ImageInfo.m_Height * (1.0f / image->m_ImageInfo.m_Width * width_new);
					if (m_UpdateImage)
					{
						image->m_ShowTransparency = !image->m_ShowTransparency;
						m_ResourceData->GetData(*m_Resource);
						m_UpdateImage = false;
					}
					image->m_ImageMutex.lock();
					if (image->m_Texture)
					{
						ImGui::Image((void*)image->m_Texture, ImVec2(width_new, height_new));
					}
					image->m_ImageMutex.unlock();

					if (ImGui::Button(std::string(ICON_STOP + std::string(" Transparent?")).c_str(), ImVec2(75, 25)))
					{
						m_UpdateImage = true;
					}
					ImGui::SameLine();
					break;
				}
				case game::GameResourceType::Local_Script:
				case game::GameResourceType::Global_Script:
				case game::GameResourceType::Verb_Script:
				{
					game::ScriptResource* script = dynamic_cast<game::ScriptResource*>(m_ResourceData);
					for (auto& instruction : script->m_Instructions)
					{
						const std::string instruction_name = instruction.m_Name + "##" + m_Resource->m_Name + "_" + m_Resource->m_Parent->m_Path + "_" + instruction.m_Name + "_" + std::to_string(instruction.m_OffsetInSCRPChunk);
						if (ImGui::CollapsingHeader(instruction_name.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowValue("Instruction", instruction.m_Name.c_str());
							ShowValue("Code", std::to_string(instruction.m_Code).c_str());
							ShowValue("Code (char)", std::string(1, instruction.m_Code).c_str());
							ShowValue("Offset", std::to_string(instruction.m_OffsetInSCRPChunk).c_str());
							const std::string args_name = "Arguments (" + std::to_string(instruction.m_Args.m_Args.size()) + ")##Args_" + m_Resource->m_Name + "_" + m_Resource->m_Parent->m_Path + "_" + instruction.m_Name + "_" + std::to_string(instruction.m_OffsetInSCRPChunk);
							if (ImGui::CollapsingHeader(args_name.c_str()))
							{
								size_t j = 1;
								for (auto& arg : instruction.m_Args.m_Args)
								{
									ImGui::Indent(IMGUI_INDENT);
									std::string val;
									switch (arg.m_ArgType)
									{
										case ArgType::ArgType_Byte:
										{
											val = std::string(1, *reinterpret_cast<const char*>(arg.m_Data));
											break;
										}
										case ArgType::ArgType_Short:
										{
											val = std::to_string(*reinterpret_cast<uint16_t*>(arg.m_Data));
											break;
										}
										case ArgType::ArgType_Ref:
										{
											val = std::to_string(*reinterpret_cast<uint16_t*>(arg.m_Data));
											break;
										}
										case ArgType::ArgType_Long:
										{
											val = std::to_string(*reinterpret_cast<uint32_t*>(arg.m_Data));
											break;
										}
										case ArgType::ArgType_String:
										{
											val = std::string(reinterpret_cast<const char*>(arg.m_Data));
											break;
										}
										case ArgType::ArgType_TalkString:
										{
											val = std::string(reinterpret_cast<const char*>(arg.m_Data));
											break;
										}
									}
									ShowValue(std::string("Arg " + std::to_string(j)), val.c_str());
									ImGui::Unindent(IMGUI_INDENT);
									j++;
								}
							}
							ImGui::Unindent(IMGUI_INDENT);
						}
					}

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
			if (ImGui::Button(std::string(ICON_SAVE + std::string(" Export")).c_str(), ImVec2(75, 25)))
			{
				if (m_ResourceData->Save(*m_Resource))
				{
					return;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(std::string(ICON_LOAD + std::string(" Replace")).c_str(), ImVec2(75, 25)))
			{
				audioSystem.Stop();

				if (m_ResourceData->Replace(*m_Resource))
				{
					project::Resource* resource = m_Resource->m_Parent;

					m_Resource = nullptr;
					delete(m_ResourceData);
					m_ResourceData = nullptr;

					project::project.ClearResources(*resource);
					project::project.LoadResource(*resource);

					LOG(logger::LOGSEVERITY_MESSAGE, "Successfully replaced resource!");
				}
			}
		}

		void GameResourceWindow::SetResource(game::GameResource& a_Resource)
		{
			if (m_ResourceData)
			{
				delete m_ResourceData;
			}

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
				case game::GameResourceType::Global_Script:
				case game::GameResourceType::Verb_Script:
				{
					m_ResourceData = new game::ScriptResource(*m_Resource);
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