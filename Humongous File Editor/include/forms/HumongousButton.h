#pragma once

#include <windows.h>
#include <cstdint>

#include "file/FileType.h"
#include "file/ResourceType.h"

public ref class HumongousButton : public System::Windows::Forms::Button
{
public:
	size_t offset = 0;
	bool special = true;
	HumongousFileEditor::files::FileType fileType;
	HumongousFileEditor::files::ResourceType resourceType;
};