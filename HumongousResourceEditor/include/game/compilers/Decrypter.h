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
		class Decrypter : Compiler
		{
		public:
			static bool Decrypt(project::Resource& a_Resource);
		};
	}
}