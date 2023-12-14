#pragma once

#include <vector>

#include "./GameResource.h"

namespace resource_editor
{
	namespace project
	{
		class Resource;
	}

	namespace game
	{
		class Compiler
		{
		public:
			virtual bool Decompile(project::Resource& a_Resource, std::vector<GameResource>& a_Resources) = 0;
		};
	}
}