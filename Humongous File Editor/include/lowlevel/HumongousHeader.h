#pragma once

#include <corecrt_malloc.h>

#include "lowlevel/Defines.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
#pragma pack(push, 1)
		/*
		 * WHAT IS THIS FILE?
		 * Every humongous chunk starts the same, with a header containing the name and chunk size.
		 */
		class HumongousHeader
		{
		public:
			HumongousHeader() = default;
			HumongousHeader(const HumongousHeader& rhs);
			HumongousHeader(HumongousHeader* a_DataBuffer);

			unsigned char chunk_id[CHUNK_ID_SIZE] = {};
			unsigned char chunkSize[sizeof(uint32_t)] = {};

			uint32_t ChunkSize(bool isBigEndian = true) const;

			void SetChunkSize(uint32_t chunkSize, bool toBigEndian = true);
		};

		using ChunkHeader = HumongousHeader;
#pragma pack(pop)
	}
}