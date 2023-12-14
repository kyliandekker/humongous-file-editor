// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "utils/abstractions.h"
#include <windows.h>
#include <ShlObj_core.h>

namespace resource_editor
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
		bool PickContainer(std::string& path)
		{
			return genericFileOpen(path, CLSID_FileOpenDialog, FOS_PICKFOLDERS);
		}

		bool PickFile(std::string& path, const std::vector<COMDLG_FILTERSPEC>& filters)
		{
			return genericFileOpen(path, CLSID_FileOpenDialog, 0, filters);
		}
		bool SaveFile(std::string& path, int* choice, const std::vector<COMDLG_FILTERSPEC>& filters)
		{
			return false;
		}
		bool SaveFolder(std::string& path)
		{
			return false;
		}
	}
}