#include "lowlevel/HumongousChunks.h"

#include <cstdint>
#include <cstring>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		SGHD_Chunk::SGHD_Chunk(SGHD_Chunk* a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				num_of_songs = a_DataBuffer->num_of_songs;
				memcpy(unknown1, a_DataBuffer->unknown1, sizeof(unknown1));
			}
		}

		SBNG_Chunk::SBNG_Chunk(const SBNG_Chunk& rhs) : HumongousHeader(rhs)
		{
			data = reinterpret_cast<unsigned char*>(malloc(rhs.ChunkSize()));
			if (data != nullptr)
				memcpy(data, rhs.data, rhs.ChunkSize());
		}

		SBNG_Chunk::SBNG_Chunk(SBNG_Chunk* a_DataBuffer) : HumongousHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				// Data is a pointer to everything after the header.
				data = reinterpret_cast<unsigned char*>(utils::add(a_DataBuffer, sizeof(SBNG_Chunk) - sizeof(data)));
			}
		}

		SGEN_Chunk::SGEN_Chunk(SGEN_Chunk* a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				id = a_DataBuffer->id;
				song_pos = a_DataBuffer->song_pos;
				song_size = a_DataBuffer->song_size;
				unknown2 = a_DataBuffer->unknown2;
			}
		}

		HSHD_Chunk::HSHD_Chunk(HSHD_Chunk* a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				memcpy(unknown1, a_DataBuffer->unknown1, sizeof(unknown1));
				unknown2 = a_DataBuffer->unknown2;
				unknown3 = a_DataBuffer->unknown3;
				sample_rate = a_DataBuffer->sample_rate;
				memcpy(unknown4, a_DataBuffer->unknown4, sizeof(unknown4));
				memcpy(unknown5, a_DataBuffer->unknown5, sizeof(unknown5));
				unknown6 = a_DataBuffer->unknown6;
			}
		}

		SDAT_Chunk::SDAT_Chunk(const SDAT_Chunk& rhs) : HumongousHeader(rhs)
		{
			data = reinterpret_cast<unsigned char*>(malloc(rhs.ChunkSize()));
			if (data != nullptr)
				memcpy(data, rhs.data, rhs.ChunkSize());
		}

		SDAT_Chunk::SDAT_Chunk(SDAT_Chunk* a_DataBuffer) : HumongousHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				// Data is a pointer to everything after the header.
				data = reinterpret_cast<unsigned char*>(utils::add(a_DataBuffer, sizeof(SDAT_Chunk) - sizeof(data)));
			}
		}

		SCRP_Chunk::SCRP_Chunk(const SCRP_Chunk& rhs) : HumongousHeader(rhs)
		{
			data = reinterpret_cast<unsigned char*>(malloc(rhs.ChunkSize()));
			if (data != nullptr)
				memcpy(data, rhs.data, rhs.ChunkSize());
		}

		SCRP_Chunk::SCRP_Chunk(SCRP_Chunk* a_DataBuffer) : HumongousHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				// data is a pointer to everything after the header.
				data = reinterpret_cast<unsigned char*>(utils::add(a_DataBuffer, sizeof(SCRP_Chunk) - sizeof(data)));
			}
		}

		BMAP_Chunk::BMAP_Chunk(const BMAP_Chunk& rhs) : HumongousHeader(rhs)
		{
			transparency = rhs.transparency;
			fill_color = rhs.fill_color;
			data = reinterpret_cast<unsigned char*>(malloc(rhs.ChunkSize()));
			if (data != nullptr)
				memcpy(data, rhs.data, rhs.ChunkSize());
		}

		BMAP_Chunk::BMAP_Chunk(BMAP_Chunk* a_DataBuffer) : HumongousHeader(a_DataBuffer)
		{
			transparency = a_DataBuffer->transparency;
			fill_color = a_DataBuffer->fill_color;
			if (a_DataBuffer != nullptr)
			{
				// data is a pointer to everything after the header.
				data = reinterpret_cast<unsigned char*>(utils::add(a_DataBuffer, sizeof(BMAP_Chunk) - sizeof(data)));
			}
		}
	}
}