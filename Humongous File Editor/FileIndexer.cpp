#include "FileIndexer.h"

#include <assert.h>

#include "lowlevel/FileContainer.h"
#include "HumongousEditorForm.h"
#include "lowlevel/utils.h"
#include "lowlevel/HumongousChunks.h"
#include "forms/HumongousNode.h"
#include <vector>
#include <map>

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
2			std::string chunk_id_name = std::string(reinterpret_cast<char*>(chunk_id));
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
			if (close)
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

		std::string recurse(Node& parent, int recursion, uint32_t s = -1)
		{
			std::string text;
			if (!parent.null)
			{
				if (s != -1 && parent.offset == s)
					return text;

				Node child = parent.Child();
				text += getOpenChunkText(parent.chunk_id, parent.offset, parent.ChunkSize(), recursion, child.null);

				text += recurse(child, recursion + 1, parent.offset + parent.ChunkSize());

				if (!child.null)
					text += getCloseChunkText(parent.chunk_id, recursion);

				Node next = parent.Next();
				text += recurse(next, recursion, s);
			}

			return text;
		}

		void FileIndexer::Read(const char* path)
		{
			FileContainer fc = FileContainer(path);
			Node root = fc.Start();

			std::string root_chunks[4]
			{
				TLKB_CHUNK_ID,
				SONG_CHUNK_ID,
				MAXS_CHUNK_ID,
				MAXS_CHUNK_ID,
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

				//uint32_t talk_size = sizeof(TALK_Chunk) + sizeof(HSHD_Chunk) + sizeof(SDAT_Chunk) - 8 + 20;
				//unsigned char chunk_size_n[CHUNK_ID_SIZE];
				//fwrite(TLKB_CHUNK_ID, CHUNK_ID_SIZE, 1, file);
				//uint32_t size = talk_size + sizeof(TLKB_Chunk);
				//memcpy(chunk_size_n, &size, sizeof(uint32_t));
				//utils::reverseBytes(chunk_size_n, sizeof(uint32_t));
				//fwrite(chunk_size_n, sizeof(uint32_t), 1, file);
				//fwrite(TALK_CHUNK_ID, CHUNK_ID_SIZE, 1, file);
				//memcpy(chunk_size_n, &talk_size, sizeof(uint32_t));
				//utils::reverseBytes(chunk_size_n, sizeof(uint32_t));
				//fwrite(chunk_size_n, sizeof(uint32_t), 1, file);
				//fwrite(HSHD_CHUNK_ID, CHUNK_ID_SIZE, 1, file);
				//size = sizeof(HSHD_Chunk);
				//memcpy(chunk_size_n, &size, sizeof(uint32_t));
				//utils::reverseBytes(chunk_size_n, sizeof(uint32_t));
				//fwrite(chunk_size_n, sizeof(uint32_t), 1, file);
				//fseek(file, sizeof(HSHD_Chunk) - sizeof(HumongousHeader), SEEK_CUR);
				//fwrite(SDAT_CHUNK_ID, CHUNK_ID_SIZE, 1, file);
				//size = sizeof(SDAT_Chunk) - 8 + 20;
				//memcpy(chunk_size_n, &size, sizeof(uint32_t));
				//utils::reverseBytes(chunk_size_n, sizeof(uint32_t));
				//fwrite(chunk_size_n, sizeof(uint32_t), 1, file);
				//unsigned char* d = reinterpret_cast<unsigned char*>(malloc(20));
				//memset(d, 0, 20);
				//fwrite(d, 20, 1, file);

				//fclose(file);

				//return;

				FileContainer fc = FileContainer(path);

				struct high_chunk
				{
					uint32_t offset;
					unsigned char chunk_id[CHUNK_ID_SIZE] = {};
				};

				std::vector<high_chunk> offsets;

				std::map<std::string, uint32_t> entries;

				Node n = fc.Start();

				std::string text = recurse(root, 0, -1);

				//for (auto i : entries)
				//{
				//	std::string f = i.first + ": " + std::to_string(i.second) + "\n";
				//	fwrite(f.c_str(), f.length(), 1, file);
				//}

				//fwrite("\n", 1, 1, file);

				fwrite(text.c_str(), text.length(), 1, file);

				fclose(file);
			}
		}
	}
}