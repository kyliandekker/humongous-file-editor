#include "abstractions/Abstractions.h"

#include <windows.h>
#include <ShlObj_core.h>

namespace HumongousFileEditor
{
	namespace abstractions
	{
		bool OpenWFile(std::string& path, LPCWSTR filter)
		{
			OPENFILENAME ofn;
			TCHAR sz_file[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = sz_file;
			ofn.nMaxFile = sizeof(sz_file);

			ofn.lpstrFilter = filter;
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameW(&ofn))
			{
				const auto cpath = new char[wcslen(ofn.lpstrFile) + 1];
				wsprintfA(cpath, "%S", ofn.lpstrFile);

				path = cpath;

				delete[] cpath;

				return true;
			}
			return false;
		}
		bool SaveWFile(std::string& path, int* choice, LPCWSTR filter)
		{
			OPENFILENAME ofn;
			TCHAR sz_file_2[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = sz_file_2;
			ofn.nMaxFile = sizeof(sz_file_2);
			ofn.lpstrFilter = filter;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetSaveFileNameW(&ofn))
			{
				const auto cpath = new char[wcslen(ofn.lpstrFile) + 1];
				wsprintfA(cpath, "%S", ofn.lpstrFile);

				path = cpath;
				if (choice != nullptr)
					*choice = ofn.nFilterIndex;

				delete[] cpath;

				return true;
			}
			return false;
		}

		static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM, LPARAM lpData)
		{

			if (uMsg == BFFM_INITIALIZED)
			{
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
			}

			return 0;
		}

		bool SaveFolder(std::string& path)
		{
			TCHAR cpath[MAX_PATH];

			const char* path_param;

			BROWSEINFO bi = { 0 };
			bi.lpszTitle = L"Browse for folder...";
			bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
			bi.lpfn = BrowseCallbackProc;
			bi.lParam = (LPARAM)path_param;

			LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

			if (pidl != 0)
			{
				//get the name of the folder and put it in path
				SHGetPathFromIDList(pidl, cpath);

				//free memory used
				IMalloc* imalloc = 0;
				if (SUCCEEDED(SHGetMalloc(&imalloc)))
				{
					imalloc->Free(pidl);
					imalloc->Release();

					std::wstring wpath(&cpath[0]);
					path = std::string(wpath.begin(), wpath.end());

					return true;
				}
			}
			return false;
		}
	}
}