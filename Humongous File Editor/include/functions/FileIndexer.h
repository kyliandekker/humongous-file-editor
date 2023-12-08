#pragma once

#include <string>

#include "functions/FileFunction.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileIndexer : public FileFunction
		{
		public:
			bool Save(std::string loadPath, std::string savePath);
		};
	}
}