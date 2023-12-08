#pragma once

#include <cstdint>
#include <string>

#include "ArgsAllocator.h"

namespace HumongousFileEditor
{
	struct instruction
	{
		uint8_t code;
		std::string name;
		size_t offset_in_scrp_chunk;
		ArgsAllocator args;
		size_t scrp_offset;
	};
}