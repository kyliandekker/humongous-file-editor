#include "lowlevel/HumongousHeader.h"

#include <cstdint>
#include <cstring>

#include "lowlevel/utils.h"

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
			uint32_t size32 = 0;
			memcpy(&size32, &chunkSize, sizeof(uint32_t));
			if (isBigEndian)
				size32 = utils::reverseBytesC<uint32_t>(reinterpret_cast<unsigned char*>(&size32));
			return size32;
		}

		void HumongousHeader::SetChunkSize(uint32_t chunk_size, bool toBigEndian)
		{
			uint32_t size32 = static_cast<uint32_t>(chunk_size);
			memcpy(chunkSize, reinterpret_cast<unsigned char*>(&size32), sizeof(uint32_t));
			if (toBigEndian)
				utils::reverseBytes(chunkSize, sizeof(uint32_t));
		}
	}
}