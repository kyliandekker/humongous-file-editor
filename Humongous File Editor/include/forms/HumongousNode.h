#pragma once

#include <windows.h>
#include <cstdint>

#include "file/FileType.h"

namespace HumongousFileEditor
{
	public ref class HumongousNode : public System::Windows::Forms::TreeNode
	{
	public:
		size_t offset = 0;
		size_t lflf = 0;
		files::FileType fileType;
	};
}