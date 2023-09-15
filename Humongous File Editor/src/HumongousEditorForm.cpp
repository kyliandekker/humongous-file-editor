#include "HumongousEditorForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
void Main(array<String^>^ args)
{
#ifdef _DEBUG
	AllocConsole();
	FILE* fConsole = nullptr;
	freopen_s(&fConsole, "CONOUT$", "w", stdout);
#endif

	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	HumongousFileEditor::HumongousEditorForm form;
	Application::Run(% form);
#ifdef _DEBUG
	fclose(fConsole);
#endif
}