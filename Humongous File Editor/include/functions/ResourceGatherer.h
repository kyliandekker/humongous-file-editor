#pragma once

#include <string>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class ResourceGatherer
		{
		public:
			void Read(const char* path);
		};
	}
}