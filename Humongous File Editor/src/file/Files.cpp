#include "file/Files.h"

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include <wtypes.h>
#include <shtypes.h>
#include <shlobj_core.h>

HumongousFileEditor::files::Files HumongousFileEditor::files::FILES;

namespace HumongousFileEditor
{
	namespace files
	{
		chunk_reader::FileContainer* Files::Read(const char* path)
		{
			// Check if we recognize the extension. If not, do not load it at all.
			files::FileType fileType = files::getFileTypeByExtension(path);
			if (fileType == files::FileType_Unknown)
				return nullptr;

			// Load the file.
			chunk_reader::FileContainer* fc = new chunk_reader::FileContainer(path);
			fc->fileType = fileType;
			fc->path = path;

			// Load the first header.
			chunk_reader::ChunkInfo header = fc->GetChunkInfo(0);

			switch (fileType)
			{
				case files::FileType_A:
				{
					if (utils::chunkcmp(header.chunk_id, chunk_reader::LECF_CHUNK_ID) != 0)
					{
						// If the first check fails, then decrypt. Maybe we already have a decrypted file, in which case it'd succeed.
						fc->Decrypt(0x69);

						// If after decryption it still does not start with a (a) chunk, return and give an error.
						header = fc->GetChunkInfo(0);
						if (utils::chunkcmp(header.chunk_id, chunk_reader::LECF_CHUNK_ID) != 0)
							return nullptr;
					}

					if (a != nullptr)
						delete a;
					a = fc;
					break;
				}
				case files::FileType_HE2:
				{
					// File does not start with HE2 chunk.
					if (utils::chunkcmp(header.chunk_id, chunk_reader::TLKB_CHUNK_ID) != 0)
						return nullptr;

					if (he2 != nullptr)
						delete he2;
					he2 = fc;
					break;
				}
				case files::FileType_HE4:
				{
					// File does not start with HE4 chunk.
					if (utils::chunkcmp(header.chunk_id, chunk_reader::SONG_CHUNK_ID) != 0)
						return nullptr;

					if (he4 != nullptr)
						delete he4;
					he4 = fc;
					break;
				}
				case files::FileType_HE0:
				{
					if (utils::chunkcmp(header.chunk_id, chunk_reader::MAXS_CHUNK_ID) != 0)
					{
						// If the first check fails, then decrypt. Maybe we already have a decrypted file, in which case it'd succeed.
						fc->Decrypt(0x69);

						// If after decryption it still does not start with a HE0 chunk, return and give an error.
						header = fc->GetChunkInfo(0);
						if (utils::chunkcmp(header.chunk_id, chunk_reader::MAXS_CHUNK_ID) != 0)
							return nullptr;
					}

					if (he0 != nullptr)
						delete he0;
					he0 = fc;
					break;
				}
				default:
				{
					return nullptr;
				}
			}

			return fc;
		}
		static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
		{

			if (uMsg == BFFM_INITIALIZED)
			{
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
			}

			return 0;
		}
		bool Files::SaveAs()
		{
			TCHAR path[MAX_PATH];

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
				SHGetPathFromIDList(pidl, path);

				//free memory used
				IMalloc* imalloc = 0;
				if (SUCCEEDED(SHGetMalloc(&imalloc)))
				{
					imalloc->Free(pidl);
					imalloc->Release();

					if (he4 != nullptr)
					{
						std::wstring wpath(&path[0]);
						std::string name = getFileName(he4->path);
						std::string path_name = std::string(wpath.begin(), wpath.end()) + "\\" + name;
						
						FILE* file = nullptr;
						fopen_s(&file, path_name.c_str(), "wb");
						fwrite(he4->data, he4->size, 1, file);
						fclose(file);
					}
					if (he2 != nullptr)
					{
						std::wstring wpath(&path[0]);
						std::string name = getFileName(he2->path);
						std::string path_name = std::string(wpath.begin(), wpath.end()) + "\\" + name;
						
						FILE* file = nullptr;
						fopen_s(&file, path_name.c_str(), "wb");
						fwrite(he2->data, he2->size, 1, file);
						fclose(file);
					}
					if (he0 != nullptr)
					{
						std::wstring wpath(&path[0]);
						std::string name = getFileName(he0->path);
						std::string path_name = std::string(wpath.begin(), wpath.end()) + "\\" + name;
						
						FILE* file = nullptr;
						fopen_s(&file, path_name.c_str(), "wb");
						he0->Decrypt();
						fwrite(he0->data, he0->size, 1, file);
						he0->Decrypt();
						fclose(file);
					}
					if (a != nullptr)
					{
						std::wstring wpath(&path[0]);
						std::string name = getFileName(a->path);
						std::string path_name = std::string(wpath.begin(), wpath.end()) + "\\" + name;
						
						FILE* file = nullptr;
						fopen_s(&file, path_name.c_str(), "wb");
						a->Decrypt();
						fwrite(a->data, a->size, 1, file);
						a->Decrypt();
						fclose(file);
					}

					return true;
				}
			}
			return false;
		}
		bool Files::Save()
		{
			if (he4 != nullptr)
			{
				FILE* file = nullptr;
				fopen_s(&file, he4->path.c_str(), "wb");
				fwrite(he4->data, he4->size, 1, file);
				fclose(file);
			}
			if (he2 != nullptr)
			{
				FILE* file = nullptr;
				fopen_s(&file, he2->path.c_str(), "wb");
				fwrite(he2->data, he2->size, 1, file);
				fclose(file);
			}
			if (he0 != nullptr)
			{
				FILE* file = nullptr;
				fopen_s(&file, he0->path.c_str(), "wb");
				he0->Decrypt();
				fwrite(he0->data, he0->size, 1, file);
				he0->Decrypt();
				fclose(file);
			}
			if (a != nullptr)
			{
				FILE* file = nullptr;
				fopen_s(&file, a->path.c_str(), "wb");
				a->Decrypt();
				fwrite(a->data, a->size, 1, file);
				a->Decrypt();
				fclose(file);
			}
			return true;
		}
        chunk_reader::FileContainer* Files::getFile(FileType fileType)
        {
			switch (fileType)
			{
				case files::FileType_A:
					return a;
				case files::FileType_HE2:
					return he2;
				case files::FileType_HE4:
					return he4;
				case files::FileType_HE0:
					return he0;
			}
			return nullptr;
        }
	}
}