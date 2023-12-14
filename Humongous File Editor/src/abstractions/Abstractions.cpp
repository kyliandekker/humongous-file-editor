#include "abstractions/Abstractions.h"

#include <windows.h>
#include <ShlObj_core.h>

namespace HumongousFileEditor
{
	namespace abstractions
	{
		bool genericFileOpen(std::string& path, const IID rclsid, FILEOPENDIALOGOPTIONS options, const std::vector<COMDLG_FILTERSPEC>& filters = {})
		{
			CoInitialize(nullptr);

			IFileDialog* pfd;
			HRESULT hr = CoCreateInstance(rclsid, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
			pfd->SetFileTypes(filters.size(), filters.data());
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

						std::wstring wide(pszPath);
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
		bool OpenWFile(std::string& path, const std::vector<COMDLG_FILTERSPEC>& filters)
		{
			return genericFileOpen(path, CLSID_FileOpenDialog, 0, filters);
		}
		bool SaveWFile(std::string& path, int* choice, const std::vector<COMDLG_FILTERSPEC>& filters)
		{
			return genericFileOpen(path, CLSID_FileSaveDialog, 0, filters);
		}

		bool SaveFolder(std::string& path)
		{
			return false;
		}

        bool OpenFolder(std::string& path)
        {
			return genericFileOpen(path, CLSID_FileOpenDialog, FOS_PICKFOLDERS);
        }
	}
}