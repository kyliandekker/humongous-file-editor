#pragma once

#include "imgui/tools/BaseTool.h"
#include "imgui/tools/ResourcesWindow.h"
#include "project/ResourceType.h"

#include <array>

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
			void RenderResource(project::Resource& a_Resource, bool& a_ShowPopUp);
			void Render() override;

			std::array<project::Resource*, 6> m_LoadedResources
			{
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
			};
		private:
			project::Resource* m_SelectedResource = nullptr;
		};
		extern ExplorerTool explorer;
	}
}