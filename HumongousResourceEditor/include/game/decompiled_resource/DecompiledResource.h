#pragma once

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class DecompiledResource
		{
		public:
			virtual bool GetData(game::GameResource& a_Resource) = 0;
		};
	}
}