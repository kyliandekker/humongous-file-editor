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
			static void SetProgressBar(System::Windows::Forms::ToolStripProgressBar^ progressBar, size_t fvalue);
		};
	};
};