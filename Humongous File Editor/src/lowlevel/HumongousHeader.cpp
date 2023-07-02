#include "lowlevel/HumongousHeader.h"

#include <cstdint>
#include <cstring>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		HumongousHeader::HumongousHeader(const HumongousHeader& rhs)
		{
			memcpy(chunk_id, rhs.chunk_id, CHUNK_ID_SIZE);
			memcpy(chunkSize, rhs.chunkSize, sizeof(uint32_t));
		}

		HumongousHeader::HumongousHeader(HumongousHeader* a_DataBuffer)
		{
			memcpy(chunk_id, a_DataBuffer->chunk_id, CHUNK_ID_SIZE);
			memcpy(chunkSize, a_DataBuffer->chunkSize, sizeof(uint32_t));
		}

		uint32_t HumongousHeader::ChunkSize(bool isBigEndian) const
		{
			unsigned char size[sizeof(uint32_t)];
			memcpy(&size, chunkSize, sizeof(uint32_t));
			return isBigEndian ? utils::reverseBytesC<uint32_t>(size) : reinterpret_cast<uint32_t>(&size);
		}

		void HumongousHeader::SetChunkSize(uint32_t chunkSize, bool toBigEndian)
		{
			memcpy(this->chunkSize, reinterpret_cast<unsigned char*>(chunkSize), sizeof(uint32_t));
			if (toBigEndian)
				utils::reverseBytes(this->chunkSize, sizeof(uint32_t));
		}
	}
}