#pragma once

#include <string>

#include "file/ResourceType.h"

namespace HumongousFileEditor
{
	namespace files
	{
		class Resource
		{
		public:
			size_t offset = 0;
			std::string name = "";
			ResourceType type;
		};
	}
}