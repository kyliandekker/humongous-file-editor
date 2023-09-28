#include "functions/ChunkFunctions.h"

#include <algorithm>
#include <windows.h>

#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		constexpr int LOOKUP_LIMIT = 15;

		void ChunkFunctions::SetProgressBar(System::Windows::Forms::ToolStripProgressBar^ progressBar, size_t fvalue)
		{
			size_t value = floor(fvalue);
			value = std::clamp(static_cast<int>(value), 0, 100);
			size_t higherValue = value + 1;
			higherValue = std::clamp(static_cast<int>(higherValue), 0, 100);
			progressBar->Value = static_cast<int>(higherValue);
			progressBar->Value = static_cast<int>(value);
		}
	}
}