#pragma once

#include "lowlevel/FileContainer.h"
#include "FileType.h"

namespace HumongousFileEditor
{
	namespace files
	{
		class Files
		{
		public:
			chunk_reader::FileContainer* Read(const char* path);

			chunk_reader::FileContainer* he2 = nullptr;
			chunk_reader::FileContainer* he4 = nullptr;
			chunk_reader::FileContainer* a = nullptr;
		};
		extern Files FILES;
	}
}