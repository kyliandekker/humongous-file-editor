#pragma once

#include "imgui/tools/BaseTool.h"
#include "imgui/tools/ResourcesWindow.h"
#include "project/ResourceType.h"

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
			void RenderResource(project::Resource& a_Resource);
            void Render() override;
			bool m_ShowPopUp = false;
			bool m_DoubleClick = false;
		private:
			project::Resource* m_SelectedResource = nullptr;
		};
		extern ExplorerTool explorerWindow;
	}
}