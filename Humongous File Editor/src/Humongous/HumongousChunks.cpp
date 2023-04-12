#include "Humongous/HumongousChunks.h"

#include <corecrt_malloc.h>

namespace HumongousFileEditor
{
	SBNG_Chunk::~SBNG_Chunk()
	{
		if (data != nullptr)
			free(data);
	}

    SDAT_Chunk::SDAT_Chunk(const SDAT_Chunk& rhs) : ChunkHeader(rhs)
	{
		if (&rhs != this && rhs.data != nullptr)
		{
			data = reinterpret_cast<unsigned char*>(malloc(rhs.chunkSize - sizeof(ChunkHeader)));
			if (data != nullptr)
			{
				memcpy(data, rhs.data, rhs.chunkSize - sizeof(ChunkHeader));
			}
		}
	}

    SDAT_Chunk::~SDAT_Chunk()
	{
		if (data != nullptr)
			free(data);
	}
}