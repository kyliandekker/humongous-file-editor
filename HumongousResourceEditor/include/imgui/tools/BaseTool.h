#pragma once

#include <string>

#include <imgui/imgui.h>

namespace resource_editor
{
	namespace imgui
	{
		class BaseTool
		{
		public:
			BaseTool(ImGuiWindowFlags a_Flags, std::string a_Name, bool a_FullScreen = false);
			~BaseTool();

			static float GetRGBColor(int a_Color);
			void ShowValue(std::string a_Text, const char* a_Value);
			virtual void WindowBegin();
			virtual void WindowEnd();
			void Update();

			void SetSize(ImVec2 a_Size);
			bool IsFullScreen() const;

			void SetEnabled(bool a_Enabled);
			void ToggleEnabled();
			std::string GetName() const;
			bool IsEnabled() const;
		protected:
			virtual void Render() = 0;

			ImGuiWindowFlags m_Flags = 0;
			ImVec2 m_Size = ImVec2(0, 0);
			std::string m_Name;
			bool m_FullScreen = false;
			bool m_Enabled = true;
		};
	}
}