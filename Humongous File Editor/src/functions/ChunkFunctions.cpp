#include "functions/ChunkFunctions.h"

#include <algorithm>
#include <windows.h>

#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "HumongousEditorForm.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		constexpr int LOOKUP_LIMIT = 15;

		void ChunkFunctions::SetProgressBar(const char* title, double fvalue)
		{
			return;

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
			form->ShowProgressBar();

			form->progressBar.actionText->Text = gcnew System::String(title);
			form->progressBar.actionText->Refresh();
			form->progressBar.progressBar1->Refresh();

			size_t value = static_cast<size_t>(floor(fvalue));
			value = std::clamp(static_cast<int>(value), 0, 100);
			size_t higherValue = value + 1;
			higherValue = std::clamp(static_cast<int>(higherValue), 0, 100);
			form->progressBar.progressBar1->Value = static_cast<int>(higherValue);
			form->progressBar.progressBar1->Value = static_cast<int>(value);

			form->progressBar.progressBar1->Invalidate();
			form->progressBar.progressBar1->Update();
		}

		void ChunkFunctions::SetGlobalProgressBar(const char* title, double fvalue)
		{
			return; 

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
			form->ShowProgressBar();

			form->progressBar.programText->Text = gcnew System::String(title);
			form->progressBar.programText->Refresh();
			form->progressBar.progressBar2->Refresh();

			size_t value = static_cast<size_t>(floor(fvalue));
			value = std::clamp(static_cast<int>(value), 0, 100);
			size_t higherValue = value + 1;
			higherValue = std::clamp(static_cast<int>(higherValue), 0, 100);
			form->progressBar.progressBar2->Value = static_cast<int>(higherValue);
			form->progressBar.progressBar2->Value = static_cast<int>(value);

			form->progressBar.progressBar2->Invalidate();
			form->progressBar.progressBar2->Update();
		}

		void ChunkFunctions::HideProgressBar()
		{
			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
			form->HideProgressBar();
		}
	}
}