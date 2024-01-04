#include "imgui/tools/MainWindow.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>

#include "project/Project.h"
#include "utils/abstractions.h"
#include "utils/string.h"

namespace resource_editor
{
	namespace imgui
	{
		MainWindow::MainWindow(std::vector<BaseTool*>& a_Tools) : BaseTool(ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking, "DockSpace", true), m_Tools(a_Tools)
		{
		}

		void OpenProject()
		{
			std::string path;
			std::vector<COMDLG_FILTERSPEC> filters =
			{
				{ L"Humongous Index Files (*.HE0)", L"*.HE0" }
			};
			if (abstractions::PickFile(path, filters))
			{
				project::project.m_Folder.m_Path = string_extensions::getPath(path);
				project::project.m_Folder.m_FoldedOut = true;
				project::project.m_Folder.Scan();
			}
		}

		void MainWindow::Render()
		{
			ImGui::DockSpace(ImGui::GetID("DockSpace"));
			ImGui::BeginMainMenuBar();
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", ""))
				{
					OpenProject();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
}