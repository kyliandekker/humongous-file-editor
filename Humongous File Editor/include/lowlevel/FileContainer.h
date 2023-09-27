#pragma once

#include <string>
#include <vector>
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
			ChunkInfo GetParent(size_t offset) const;
			std::vector<ChunkInfo> GetChildren(size_t offset) const;
			void Replace(size_t offset, unsigned char* new_data, size_t new_size);
			void Decrypt(char key);

			files::FileType fileType;
			size_t size = 0;
			std::string path;
			unsigned char* data = nullptr;
		};
	}
}