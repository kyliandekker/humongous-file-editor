#pragma once

#include <vector>
#include <map>

#include "imgui/tools/BaseTool.h"
#include "imgui/imgui_helpers.h"

namespace resource_editor
{
	namespace imgui
	{
		class MainWindow : public BaseTool
		{
		public:
			MainWindow(std::vector<BaseTool*>& a_Tools);
			void Render() override;
		private:
			std::vector<BaseTool*>& m_Tools;
		};
	}
}