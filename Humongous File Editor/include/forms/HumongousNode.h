#pragma once

#include <windows.h>
#include <cstdint>

#include "file/FileType.h"
#include "file/ResourceType.h"

namespace HumongousFileEditor
{
	public ref class HumongousNode : public System::Windows::Forms::TreeNode
	{
	public:
		size_t offset = 0;
		bool special = false;
		files::FileType fileType;
		files::ResourceType resourceType;
	};
}