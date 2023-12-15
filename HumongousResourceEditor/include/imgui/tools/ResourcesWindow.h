#pragma once

#include <vector>

#include "imgui/tools/BaseTool.h"
#include "game/GameResource.h"
#include "GameResourceWindow.h"

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
			ResourcesWindow(std::string a_Name, project::Resource& a_Resource);
			ResourcesWindow(const ResourcesWindow& rhs) = default;
			void Render() override;
		private:
			project::Resource* m_Resource = nullptr;
			game::GameResource* m_SelectedResource = nullptr;

			std::vector<GameResourceWindow> m_Windows;
		};
	}
}