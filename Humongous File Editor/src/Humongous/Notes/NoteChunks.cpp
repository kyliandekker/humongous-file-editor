#include "Humongous/Notes/NoteChunks.h"

#include <corecrt_malloc.h>

namespace HumongousFileEditor
{
	namespace notes
	{
		NOTE_Chunk::NOTE_Chunk(const NOTE_Chunk& rhs) : ChunkHeader(rhs)
		{
			if (&rhs != this && rhs.data != nullptr)
			{
				data = reinterpret_cast<char*>(malloc(rhs.chunkSize - sizeof(ChunkHeader)));
				if (data != nullptr)
				{
					memcpy(data, rhs.data, rhs.chunkSize - sizeof(ChunkHeader));
				}
			}
		}

		NOTE_Chunk::~NOTE_Chunk()
		{
			if (data != nullptr)
				free(data);
		}
	}
}