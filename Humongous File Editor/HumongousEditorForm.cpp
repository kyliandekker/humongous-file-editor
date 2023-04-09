#include "HumongousEditorForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
void Main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	hfe::HumongousEditorForm form;
	Application::Run(% form);
}