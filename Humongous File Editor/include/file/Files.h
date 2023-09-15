#pragma once

#include "FileType.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;
	}
	namespace files
	{
		enum FileType;

		class Files
		{
		public:
			chunk_reader::FileContainer* Read(const char* path);
			chunk_reader::FileContainer* getFile(FileType fileType);

			chunk_reader::FileContainer* he2 = nullptr;
			chunk_reader::FileContainer* he4 = nullptr;
			chunk_reader::FileContainer* a = nullptr;
			chunk_reader::FileContainer* he0 = nullptr;
		};
		extern Files FILES;
	}
}