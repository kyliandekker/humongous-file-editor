#pragma once

#include <windows.h>
#include <cstdint>

#include "file/FileType.h"

using namespace System::Windows::Forms;
using namespace System::Drawing;

namespace HumongousFileEditor
{
	public ref class HumongousNode : public TreeNode
	{
	public:
		uint32_t offset = 0;
		files::FileType fileType;
	};
}