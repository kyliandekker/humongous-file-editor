#pragma once

#include <vector>
#include <string>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
        struct bytecode_data
        {
            std::vector<uint8_t> data_8;
            std::vector<uint16_t> data_16;
            std::vector<uint32_t> data_32;
        };
	}
}