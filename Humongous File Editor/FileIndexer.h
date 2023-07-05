#pragma once

#include <string>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileIndexer
		{
		public:
			void Read(const char* path);
		};
	}
}