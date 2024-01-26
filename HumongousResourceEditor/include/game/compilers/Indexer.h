#pragma once

#include "game/Compiler.h"

namespace resource_editor
{
	namespace project
	{
		class Resource;
	}

	namespace game
	{
		class Indexer : Compiler
		{
		public:
			static bool Create(project::Resource& a_Resource);
		};
	}
}