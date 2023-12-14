#pragma once

#include "imgui/tools/BaseTool.h"

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
			void Render() override;
		private:
			project::Resource* m_SelectedResource = nullptr;
		};
	}
}