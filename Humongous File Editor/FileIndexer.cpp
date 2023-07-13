#include "FileIndexer.h"

#include <assert.h>
#include <vector>
#include <map>
#include <stack>

#include "lowlevel/FileContainer.h"
#include "HumongousEditorForm.h"
#include "lowlevel/utils.h"
#include "lowlevel/HumongousChunks.h"
#include "forms/HumongousNode.h"
#include "lowlevel/HumongousChunkDefinitions.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		inline bool ends_with(std::string const& value, std::string const& ending)
		{
			if (ending.size() > value.size()) return false;
			return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
		}

		std::string getOpenChunkText(unsigned char* chunk_id, uint32_t offset, uint32_t total_size, uint32_t recursion, bool close)
		{
			std::string chunk_id_name = std::string(reinterpret_cast<char*>(chunk_id));
			chunk_id_name.resize(CHUNK_ID_SIZE);

			std::string text;
			for (size_t j = 0; j < recursion; j++)
				text += "\t";
			text += "<";
			text += chunk_id_name;
			text += " position=\"";
			text += std::to_string(offset);
			text += "\"";
			text += " total_size=\"";
			text += std::to_string(total_size);
			text += "\"";
			text += " size=\"";
			text += std::to_string(total_size - sizeof(HumongousHeader));
			text += "\"";
			if (!close)
				text += "/";
			text += ">\n";
			return text;
		};

		std::string getCloseChunkText(unsigned char* chunk_id, uint32_t recursion)
		{
			std::string chunk_id_name = std::string(reinterpret_cast<char*>(chunk_id));
			chunk_id_name.resize(CHUNK_ID_SIZE);

			std::string text;
			for (size_t j = 0; j < recursion; j++)
				text += "\t";
			text += "</";
			text += chunk_id_name;
			text += ">\n";
			return text;
		};

		void FileIndexer::Read(const char* path)
		{
			FileContainer fc = FileContainer(path);

			std::string root_chunks[4]
			{
				TLKB_CHUNK_ID,
				SONG_CHUNK_ID,
				MAXS_CHUNK_ID,
				LECF_CHUNK_ID,
			};

			OPENFILENAME ofn;
			TCHAR sz_file[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = sz_file;
			ofn.nMaxFile = sizeof(sz_file);
			ofn.lpstrFilter = L"\
						HTML File (*.html)\
						\0*.HTML;*.html\0";
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetSaveFileNameW(&ofn))
			{
				const auto save_path = new char[wcslen(ofn.lpstrFile) + 1];
				wsprintfA(save_path, "%S", ofn.lpstrFile);

				std::string save_path_s = std::string(save_path);

				if (!ends_with(save_path_s, ".html"))
					save_path_s += ".html";

				FILE* file = nullptr;
				fopen_s(&file, save_path_s.c_str(), "wb");
				if (file == nullptr)
				{
					printf("Cannot save file");
					return;
				}

				delete[] save_path;

				FileContainer fc = FileContainer(path);

				ChunkInfo header = fc.GetChunkInfo(0);
				std::vector<ChunkInfo> chunks;
				while (header.offset < fc.size)
				{
					std::string text;
					for (int i = chunks.size(); i-- > 0; )
						if (chunks[i].offset + chunks[i].ChunkSize() <= header.offset)
						{
							text = getCloseChunkText(chunks[i].chunk_id, chunks.size());
							fwrite(text.c_str(), text.length(), 1, file);
							chunks.erase(chunks.begin() + i);
						}
					ChunkInfo next = fc.GetNextChunk(header.offset);
					bool b = header.offset + header.ChunkSize() >= next.offset;
					if (b)
						chunks.push_back(header);
					text = getOpenChunkText(header.chunk_id, header.offset, header.ChunkSize(), chunks.size(), b);
					fwrite(text.c_str(), text.length(), 1, file);
					header = next;
				}

				fclose(file);
			}
		}
	}
}