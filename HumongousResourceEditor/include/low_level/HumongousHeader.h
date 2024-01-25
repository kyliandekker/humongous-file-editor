#pragma once

#include "low_level/Defines.h"

namespace resource_editor
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
			HumongousHeader(const char* a_ChunkID, size_t a_ChunkSize = 0, bool a_BigEndian = true);
			HumongousHeader(unsigned char* a_ChunkID, size_t a_ChunkSize = 0, bool a_BigEndian = true);
			HumongousHeader(const HumongousHeader& rhs);
			HumongousHeader(HumongousHeader* a_DataBuffer);

			unsigned char chunk_id[CHUNK_ID_SIZE] = {};
			unsigned char chunkSize[sizeof(uint32_t)] = {};

			uint32_t ChunkSize(bool a_BigEndian = true) const;
			uint32_t DataSize(bool a_BigEndian = true) const;

			void SetChunkSize(uint32_t a_ChunkSize, bool a_BigEndian = true);
		};

		using ChunkHeader = HumongousHeader;
#pragma pack(pop)
	}
}