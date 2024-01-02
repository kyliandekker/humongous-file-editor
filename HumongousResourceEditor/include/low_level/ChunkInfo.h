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
			ChunkInfo(const ChunkInfo& rhs) : HumongousHeader(rhs)
			{
				m_Offset = rhs.m_Offset;
			}
			size_t m_Offset = 0;
		};
	}
}