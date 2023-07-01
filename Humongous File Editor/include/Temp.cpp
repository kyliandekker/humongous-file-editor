#include "Temp.h"

#include <cstdio>
#include <string>

#include "lowlevel/HumongousChunks.h"
#include "lowlevel/HumongousChunkCollection.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		void Temp::Read(const char* path)
		{
			FILE* file = nullptr;

			// Open the file.
			fopen_s(&file, path, "rb");
			if (file == nullptr)
			{
				printf("Cannot open file");
				return;
			}

			fseek(file, 0, SEEK_END);
			int size = ftell(file);
			rewind(file);

			HumongousChunkCollection chunkCollection = HumongousChunkCollection(malloc(size), size);

			fread(chunkCollection.data(), size, 1, file);

			size_t total_chunks = chunkCollection.GetNumberOfChunks();
			size_t total_chunk_size = chunkCollection.size();
		}
	}
}