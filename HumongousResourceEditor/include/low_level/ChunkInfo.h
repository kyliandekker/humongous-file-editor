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
			ChunkInfo(const char* a_ChunkID) : HumongousHeader(a_ChunkID)
			{ }
			ChunkInfo(const ChunkInfo& rhs) : HumongousHeader(rhs)
			{
				m_Offset = rhs.m_Offset;
			}
			int32_t m_Offset = 0;
		};
	}
}