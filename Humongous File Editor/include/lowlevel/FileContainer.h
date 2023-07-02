#pragma once

#include <string>
#include "Node.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer
		{
		public:
			FileContainer(std::string path);
			~FileContainer();

			Node Start();
			Node NodeFromOffset(size_t offset);
			size_t size = 0;
			unsigned char* data = nullptr;
		};
	}
}