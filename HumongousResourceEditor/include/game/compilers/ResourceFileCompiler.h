#pragma once

#include "game/Compiler.h"

namespace resource_editor
{
	namespace game
	{
		class ResourceFileCompiler : Compiler
		{
		public:
			bool Decompile(project::Resource& a_Resource, std::vector<GameResource>& a_Resources) override;
		};
	}
}