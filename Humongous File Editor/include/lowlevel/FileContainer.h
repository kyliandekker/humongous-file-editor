#pragma once

#include <string>
#include "ChunkInfo.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer
		{
		public:
			FileContainer(std::string path);
			~FileContainer();

			ChunkInfo GetChunkInfo(size_t offset) const;
			ChunkInfo GetNextChunk(size_t offset) const;

			size_t size = 0;
			unsigned char* data = nullptr;
		};
	}
}