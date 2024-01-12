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
			void RenderResource(project::Resource& a_Resource);
            void Render() override;
			bool m_ShowPopUp = false;
			bool m_DoubleClick = false;

			void LoadResource(project::Resource& a_Resource);
			void LoadResourceWithPath(project::Resource& a_Resource);
            void ClearResources(project::Resource& a_Resource);
			void UnloadResource(project::ResourceType a_ResourceType);

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