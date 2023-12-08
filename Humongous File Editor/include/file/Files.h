#pragma once

#include <string>

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
			~Files();

			chunk_reader::FileContainer* Read(std::string path);
			bool SaveAs(std::string path);
			bool Save();
			chunk_reader::FileContainer* getFile(FileType fileType);

			chunk_reader::FileContainer* he0 = nullptr;
			chunk_reader::FileContainer* a = nullptr;
			chunk_reader::FileContainer* he2 = nullptr;
			chunk_reader::FileContainer* he4 = nullptr;
		};
		extern Files FILES;
	}
}