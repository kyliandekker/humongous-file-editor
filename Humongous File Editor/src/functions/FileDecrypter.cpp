#include "functions/FileDecrypter.h"

#include <windows.h>

#include "lowlevel/FileContainer.h"
#include "systems/Logger.h"
#include "lowlevel/utils.h"
#include "HumongousEditorForm.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		bool FileDecrypter::Read()
		{
			OPENFILENAME ofn;
			TCHAR sz_file[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = sz_file;
			ofn.nMaxFile = sizeof(sz_file);

			ofn.lpstrFilter = getFilter();
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameW(&ofn))
			{
				const auto path = new char[wcslen(ofn.lpstrFile) + 1];
				wsprintfA(path, "%S", ofn.lpstrFile);

				chunk_reader::FileContainer fc = chunk_reader::FileContainer(path);
				fc.Decrypt(0x69);

				HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
				form->toolProgressBar->Value = 100;

				OPENFILENAME ofn;
				TCHAR sz_file[260] = { 0 };

				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = sz_file;
				ofn.nMaxFile = sizeof(sz_file);
				ofn.lpstrFilter = getFilter();
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = nullptr;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (GetSaveFileNameW(&ofn))
				{
					const auto save_path = new char[wcslen(ofn.lpstrFile) + 1];
					wsprintfA(save_path, "%S", ofn.lpstrFile);

					files::FileType ft = files::getFileTypeByExtension(path);

					std::string extension = files::getExtensionByFileType(ft);

					std::string save_path_s = std::string(save_path);

					if (!utils::ends_with(save_path_s, extension))
						save_path_s += extension;

					FILE* file = nullptr;
					fopen_s(&file, save_path_s.c_str(), "wb");
					if (file == nullptr)
					{
						LOGF(logger::LOGSEVERITY_ERROR, "Cannot save file \"%s\".", save_path_s.c_str());
						return false;
					}

					fwrite(fc.data, fc.size, 1, file);
					fclose(file);
				}

				delete[] path;
				return true;
			}
			return false;
		}
	}
}