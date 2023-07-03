#pragma once

#include <string>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class Temp
		{
		public:
            void AddEntry(uint32_t offset, std::string name);
            void Read(const char* path);
		};
	}
}