#pragma once

#include <vector>
#include "File.h"

namespace HumongousFileEditor
{
	public class FileContainer
	{
	public:
		std::vector<File*> files;

		void Clear();
	};
	extern FileContainer fileContainer;
}