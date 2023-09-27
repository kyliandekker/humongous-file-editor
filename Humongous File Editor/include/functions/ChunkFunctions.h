#pragma once

#include <vector>
#include <string>

namespace System
{
	namespace Windows
	{
		namespace Forms
		{
			ref class ToolStripProgressBar;
		};
	};
};

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;

		class ChunkFunctions
		{
		public:
			static size_t GetOffsetChunk(FileContainer*& fc, size_t offset, std::vector<std::string> chunk_names);
			static void SetProgressBar(System::Windows::Forms::ToolStripProgressBar^ progressBar, int value);
		};
	};
};