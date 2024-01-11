#pragma once

#include <vector>

#include "imgui/tools/BaseTool.h"
#include "game/GameResource.h"

namespace resource_editor
{
	namespace project
	{
		class Resource;
	}
	namespace game
	{
		class GameResource;
	}
	namespace imgui
	{
		class ResourcesWindow : public BaseTool
		{
		public:
			ResourcesWindow();
			void RenderGameResource(game::GameResource& resource, game::GameResource*& selectedResource);
			ResourcesWindow(const ResourcesWindow& rhs) = default;
			void Render() override;
			void SetActiveTab(int a_Selected);
		private:
			game::GameResource* m_SelectedResource = nullptr;

			bool m_ShowPopUp = false;
			bool m_DoubleClick = false;
			int m_ActiveTab = 0;
		};
		extern ResourcesWindow resourcesWindow;
	}
}