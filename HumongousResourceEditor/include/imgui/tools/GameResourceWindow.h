#pragma once

#include <vector>

#include "imgui/tools/BaseTool.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource;
		class DecompiledResource;
	}
	namespace imgui
	{
		class GameResourceWindow : public BaseTool
		{
		public:
			GameResourceWindow();
			void Render() override;
			void SetResource(game::GameResource& a_Resource);

			game::GameResource* m_Resource = nullptr;
			game::DecompiledResource* m_ResourceData = nullptr;
		};
		extern GameResourceWindow gameResourceWindow;
	}
}