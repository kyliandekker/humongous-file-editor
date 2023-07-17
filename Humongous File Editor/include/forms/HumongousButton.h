#pragma once

#include <windows.h>
#include <cstdint>

#include "file/FileType.h"

using namespace System::Windows::Forms;
using namespace System::Drawing;

public ref class HumongousButton : public Button
{
public:
	uint32_t offset = 0;
	HumongousFileEditor::files::FileType fileType;
};