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
				offset = rhs.offset;
			}
			size_t offset = 0;
		};
	}
}