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
			ResourcesWindow(const ResourcesWindow& rhs) = default;
			void Render() override;
			void SetActiveTab(int a_Selected);
		private:
			game::GameResource* m_SelectedResource = nullptr;

			int m_ActiveTab = 0;
		};
		extern ResourcesWindow resourcesWindow;
	}
}