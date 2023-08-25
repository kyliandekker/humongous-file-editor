#pragma once

#include <string>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;

		class ResourceGatherer
		{
		public:
			void Read(const char* path);
			void ReadResourceFile(FileContainer*& fc);
			void ReadHE2(FileContainer*& fc);
			void ReadHE4(FileContainer*& fc);
		};
	}
}