#pragma once

#include <string>

#include "GameResourceType.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource
		{
		public:
			size_t offset = 0;
			std::string name = "";
			GameResourceType type;
		};
	}
}