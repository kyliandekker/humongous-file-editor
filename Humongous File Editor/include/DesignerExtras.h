#pragma once

#include <windows.h>

using namespace System::Windows::Forms;
using namespace System::Drawing;

public ref class HumongousButton : public Button
{
public:
	int num = 0;
};

public ref class HumongousNode : public TreeNode
{
public:
	int num = 0;
};

public ref class HumongousToolStripMenuItem : public ToolStripMenuItem
{
public:
	HumongousNode^ node;
};