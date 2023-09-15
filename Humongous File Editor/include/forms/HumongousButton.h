#pragma once

#include <windows.h>
#include <cstdint>

#include "file/FileType.h"

public ref class HumongousButton : public System::Windows::Forms::Button
{
public:
	size_t offset = 0;
	HumongousFileEditor::files::FileType fileType;
};