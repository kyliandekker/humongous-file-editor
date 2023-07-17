#pragma once

#include <string>
#include "ChunkInfo.h"
#include "file/FileType.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer
		{
		public:
			FileContainer() = default;
			FileContainer(std::string path);
			FileContainer(const FileContainer& rhs);
			~FileContainer();

			ChunkInfo GetChunkInfo(size_t offset) const;
			ChunkInfo GetNextChunk(size_t offset) const;

			files::FileType fileType;
			size_t size = 0;
			unsigned char* data = nullptr;
		};
	}
}