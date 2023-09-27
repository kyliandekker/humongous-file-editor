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

		size_t ChunkFunctions::GetOffsetChunk(FileContainer*& fc, size_t offset, std::vector<std::string> chunk_names)
		{
			ChunkInfo info = fc->GetChunkInfo(offset);
			info = fc->GetNextChunk(info.offset);

			int i = 0;
			while (i < LOOKUP_LIMIT && offset < fc->size)
			{
				for (size_t j = 0; j < chunk_names.size(); j++)
					if (utils::chunkcmp(info.chunk_id, chunk_names[j].c_str()) == 0)
						return info.offset;
				info = fc->GetNextChunk(info.offset);
			}

			return -1;
		}

		void ChunkFunctions::SetProgressBar(System::Windows::Forms::ToolStripProgressBar^ progressBar, int value)
		{
			value = std::clamp(value, 0, 100);
			int higherValue = value + 1;
			higherValue = std::clamp(higherValue, 0, 100);
			progressBar->Value = higherValue;
			progressBar->Value = value;
		}
	}
}