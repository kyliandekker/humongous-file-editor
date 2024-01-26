#include "game/compilers/Indexer.h"
#include "utils/abstractions.h"
#include "utils/string.h"
#include "low_level/ChunkInfo.h"
#include "project/Resource.h"

namespace resource_editor
{
	namespace game
	{
		std::string getOpenChunkText(unsigned char* chunk_id, size_t offset, size_t total_size, size_t recursion, bool close)
		{
			std::string chunk_id_name = std::string(reinterpret_cast<char*>(chunk_id));
			chunk_id_name.resize(CHUNK_ID_SIZE);

			std::string text;
			for (size_t j = 0; j < recursion; j++)
			{
				text += "\t";
			}
			text += "<";
			text += chunk_id_name;
			text += " position=\"";
			text += std::to_string(offset);
			text += "\"";
			text += " total_size=\"";
			text += std::to_string(total_size);
			text += "\"";
			text += " size=\"";
			text += std::to_string(total_size - sizeof(chunk_reader::HumongousHeader));
			text += "\"";
			if (!close)
			{
				text += "/";
			}
			text += ">\n";
			return text;
		};

		std::string getCloseChunkText(unsigned char* chunk_id, size_t recursion)
		{
			std::string chunk_id_name = std::string(reinterpret_cast<char*>(chunk_id));
			chunk_id_name.resize(CHUNK_ID_SIZE);

			std::string text;
			for (size_t j = 0; j < recursion; j++)
			{
				text += "\t";
			}
			text += "</";
			text += chunk_id_name;
			text += ">\n";
			return text;
		};

		bool Indexer::Create(project::Resource& a_Resource)
		{
			std::string path;
			const std::vector<COMDLG_FILTERSPEC> filters =
			{
				{ L"HTML (*.html)", L"*.HTML" }
			};

			if (!abstractions::SaveFile(path, nullptr, filters))
			{
				return false;
			}

			if (!string_extensions::ends_with(path, ".html"))
			{
				path += ".html";
			}

			std::map<std::string, uint32_t> chunks;
			chunk_reader::ChunkInfo header = a_Resource.m_FileContainer.GetChunkInfo(0);
			std::vector<chunk_reader::ChunkInfo> top_chunks;
			std::string text;
			while (header.m_Offset < a_Resource.m_FileContainer.size())
			{
				std::string chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
				chunk_id_name.resize(CHUNK_ID_SIZE);
				chunks[chunk_id_name]++;

				chunk_reader::ChunkInfo next = a_Resource.m_FileContainer.GetNextChunk(header.m_Offset);
				bool b = header.m_Offset + header.ChunkSize() > next.m_Offset;
				text += getOpenChunkText(header.chunk_id, header.m_Offset, header.ChunkSize(), top_chunks.size(), b);
				if (b)
				{
					top_chunks.push_back(header);
				}
				header = next;
				for (size_t i = top_chunks.size(); i-- > 0; )
				{
					if (top_chunks[i].m_Offset + top_chunks[i].ChunkSize() == header.m_Offset)
					{
						chunk_reader::ChunkInfo closed = top_chunks[i];
						top_chunks.erase(top_chunks.begin() + i);
						text += getCloseChunkText(closed.chunk_id, top_chunks.size());
					}
				}
			}

			FILE* file = nullptr;
			fopen_s(&file, path.c_str(), "wb");
			if (!file)
			{
				return false;
			}
			
			for (auto i : chunks)
			{
				std::string chunk_text = i.first + ": " + std::to_string(i.second) + "\n";
				fwrite(chunk_text.c_str(), chunk_text.length(), 1, file);
			}
			fwrite("\n", 1, 1, file);

			fwrite(text.c_str(), text.length(), 1, file);

			fclose(file);
			return true;
		}
	}
}