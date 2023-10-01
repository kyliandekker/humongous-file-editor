#pragma once

#include <windows.h>
#include <cstdint>

#include "file/FileType.h"
#include "file/ResourceType.h"

namespace HumongousFileEditor
{
	ref class HumongousNode;

	public ref class HumongousButton : public System::Windows::Forms::Button
	{
	public:
		size_t offset = 0;
		HumongousNode^ node;
		bool special = true;
		files::FileType fileType;
		files::ResourceType resourceType;
	};
}