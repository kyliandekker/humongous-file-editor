#pragma once

#include <windows.h>
#include <cstdint>

using namespace System::Windows::Forms;
using namespace System::Drawing;

public ref class HumongousButton : public Button
{
public:
	uint32_t offset = 0;
};