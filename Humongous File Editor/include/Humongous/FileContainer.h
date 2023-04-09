#pragma once

#include <vector>
#include "File.h"

namespace hfe
{
	public class FileContainer
	{
	public:
		std::vector<File*> files;

		void Clear();
	};
	extern FileContainer fileContainer;
}