#include "utils/abstractions.h"

#include <windows.h>
#include <ShlObj_core.h>

#include "project/ResourceType.h"

namespace resource_editor
{
	namespace abstractions
	{
		bool genericFileOpen(std::string& path, const IID rclsid, FILEOPENDIALOGOPTIONS options, const std::vector<COMDLG_FILTERSPEC>& filters = {})
		{
			CoInitialize(nullptr);

			IFileDialog* pfd;
			HRESULT hr = CoCreateInstance(rclsid, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
			pfd->SetFileTypes(static_cast<UINT>(filters.size()), filters.data());
			if (SUCCEEDED(hr))
			{
				DWORD dwOptions;
				if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
				{
					pfd->SetOptions(dwOptions | options);
				}

				if (SUCCEEDED(pfd->Show(NULL)))
				{
					IShellItem* psi;
					if (SUCCEEDED(pfd->GetResult(&psi)))
					{
						LPWSTR pszPath;
						psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszPath);

						const std::wstring wide(pszPath);
						path = std::string(wide.begin(), wide.end());

						psi->Release();
						CoTaskMemFree(pszPath);

						return true;
						psi->Release();
					}
				}
				pfd->Release();
			}
			return false;
		}

		bool PickContainer(std::string& a_Path)
		{
			return genericFileOpen(a_Path, CLSID_FileOpenDialog, FOS_PICKFOLDERS);
		}

		bool PickFile(std::string& a_Path, const std::vector<COMDLG_FILTERSPEC>& a_Filters)
		{
			return genericFileOpen(a_Path, CLSID_FileOpenDialog, 0, a_Filters);
		}

		bool SaveFile(std::string& a_Path, int*a_Choice, const std::vector<COMDLG_FILTERSPEC>& a_Filters)
		{
			return genericFileOpen(a_Path, CLSID_FileSaveDialog, 0, a_Filters);
		}

		bool SaveFolder(std::string& a_Path)
		{
			return false;
		}

        std::vector<COMDLG_FILTERSPEC> GetFilters(project::ResourceType a_ResourceType)
        {
			const std::vector<COMDLG_FILTERSPEC> possible_filters =
			{
				{ L"", L"" },
				{ L"Humongous Index Files (*.HE0)", L"*.HE0" },
				{ L"Humongous Talk Files (*.HE2)", L"*.HE2" },
				{ L"Humongous Song Files (*.HE4)", L"*.HE4" },
				{ L"", L"" },
				{ L"Humongous Resource Files (*.(A))", L"*.(A)" },
			};
			const std::vector<COMDLG_FILTERSPEC> filters =
			{
				possible_filters[(int)a_ResourceType]
			};

			return filters;
        }
	}
}