#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <imgui/imgui.h>

#include "dx9/DX9Window.h"

namespace resource_editor
{
	namespace logger
	{
		struct Message;
	}
	namespace imgui
	{
		class BaseTool;
		class MainWindow;

		class ImGuiWindow
		{
		public:
			ImGuiWindow();
			~ImGuiWindow();

			void SetHwnd(HWND a_Hwnd, WNDCLASSEX a_Wc);
			void Initialize();
			void ProcessEvents(HWND a_Hwnd, UINT a_Msg, WPARAM a_wParam, LPARAM a_lParam);
			void CreateContext();
			void CreateImGui() const;
			void Render();
			void DeleteWindow();
			void AddTool(BaseTool& a_Tool);
			void ResetDevice();
			bool IsEnabled() const;
			void Stop();
			void SetSize(ImVec2 a_Size);
			dx9::DX9Window& GetDX9();

			static void LoggerCallback(const logger::Message& a_Message);

			bool m_ShowPopUp = false;
			std::string m_PopUpText;
			std::string m_PopUpTitle;
			bool m_IsError = true;
			std::mutex m_RenderMutex;
		private:
			HWND m_Hwnd = nullptr;
			WNDCLASSEX m_Wc;
			bool m_Enabled = true;

			ImVec2 m_Size;

			dx9::DX9Window m_DX9Window;

			MainWindow* m_MainWindow = nullptr;
			std::vector<BaseTool*> m_Tools;
		};
		extern ImGuiWindow window;
	}
}