#pragma once

#include <vector>

#include "imgui/tools/BaseTool.h"
#include "game/GameResource.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource;
		class DecompiledResource;

		class TalkResource;
		class SongResource;
	}
	namespace imgui
	{
		class GameResourceWindow : public BaseTool
		{
		public:
			GameResourceWindow(game::GameResource& a_GameResource);
			~GameResourceWindow();
			void Render() override;
		private:
			game::GameResource* m_Resource = nullptr;

			game::TalkResource* m_Talk = nullptr;
			game::SongResource* m_Song = nullptr;
		};
	}
}