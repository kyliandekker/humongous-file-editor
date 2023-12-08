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
			bool Read(FileContainer*& path);
			bool Read(std::string path);
			bool ReadResourceFile(FileContainer*& fc);
			bool ReadHE2(FileContainer*& fc);
			bool ReadHE4(FileContainer*& fc);
		};
	}
}