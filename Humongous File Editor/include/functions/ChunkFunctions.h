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
			static void SetProgressBar(const char* title, double fvalue);
			static void SetGlobalProgressBar(const char* title, double fvalue);
			static void HideProgressBar();
		};
	};
};