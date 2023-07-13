#pragma once

#include "HumongousHeader.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class ChunkInfo : public HumongousHeader
		{
		public:
			ChunkInfo() = default;
			ChunkInfo(const HumongousHeader& rhs) : HumongousHeader(rhs)
			{}
			size_t offset = 0;
		};
	}
}