#pragma once

#include "imgui/tools/BaseTool.h"
#include "imgui/tools/ResourcesWindow.h"

#include <vector>

namespace resource_editor
{
	namespace project
	{
		class Resource;
	}
	namespace imgui
	{
		class ExplorerTool : public BaseTool
		{
		public:
			ExplorerTool();
			void RenderResource(project::Resource& resource, bool& showPopUp);
			void Render() override;
		private:
			project::Resource* m_SelectedResource = nullptr;

			std::vector<ResourcesWindow> m_Windows;
		};
	}
}