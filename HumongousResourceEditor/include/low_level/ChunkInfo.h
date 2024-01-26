#pragma once

#include "HumongousHeader.h"

namespace resource_editor
{
	namespace chunk_reader
	{
		class ChunkInfo : public HumongousHeader
		{
		public:
			ChunkInfo() = default;
			ChunkInfo(const char* a_ChunkID, uint32_t a_ChunkSize = 0, bool a_BigEndian = true) : HumongousHeader(a_ChunkID, a_ChunkSize, a_BigEndian)
			{ }
			ChunkInfo(unsigned char* a_ChunkID, uint32_t a_ChunkSize = 0, bool a_BigEndian = true) : HumongousHeader(a_ChunkID, a_ChunkSize, a_BigEndian)
			{ }
			ChunkInfo(const ChunkInfo& rhs) : HumongousHeader(rhs)
			{
				m_Offset = rhs.m_Offset;
			}
			int32_t m_Offset = -1;
		};
	}
}