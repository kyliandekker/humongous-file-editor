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
			virtual bool Replace(game::GameResource& a_Resource) = 0;
			virtual bool Save(game::GameResource& a_Resource) = 0;
			bool UpdateIndexAndResourceFile(game::GameResource& a_Resource);
		};
	}
}