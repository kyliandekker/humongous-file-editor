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
			bool Read();
		};
	}
}