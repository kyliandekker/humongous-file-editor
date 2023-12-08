#include "functions/FileIndexer.h"

#include <assert.h>
#include <vector>
#include <map>
#include <stack>

#include "lowlevel/FileContainer.h"
#include "HumongousEditorForm.h"
#include "lowlevel/utils.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include "systems/Logger.h"
#include "functions/ChunkFunctions.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
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
			text += std::to_string(total_size - sizeof(HumongousHeader));
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

		bool FileIndexer::Save(std::string loadPath, std::string savePath)
		{
			FileContainer m_FileContainer = FileContainer(loadPath);
			if (!utils::ends_with(savePath, ".html"))
			{
				savePath += ".html";
			}

			FILE* file = nullptr;
			fopen_s(&file, savePath.c_str(), "wb");
			if (file == nullptr)
			{
				LOGF(logger::LOGSEVERITY_ERROR, "Cannot save file \"%s\".", savePath.c_str());
				return false;
			}

			switch (m_FileContainer.fileType)
			{
				case files::FileType::FileType_A:
				case files::FileType::FileType_HE0:
				{
					m_FileContainer.Decrypt(0x69);
					break;
				}
			}

			std::map<std::string, uint32_t> chunks;
			ChunkInfo header = m_FileContainer.GetChunkInfo(0);
			std::vector<ChunkInfo> top_chunks;

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];

			std::string text;
			while (header.offset < m_FileContainer.size)
			{
				std::string chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
				chunk_id_name.resize(CHUNK_ID_SIZE);
				chunks[chunk_id_name]++;

				ChunkInfo next = m_FileContainer.GetNextChunk(header.offset);
				bool b = header.offset + header.ChunkSize() > next.offset;
				text += getOpenChunkText(header.chunk_id, header.offset, header.ChunkSize(), top_chunks.size(), b);
				if (b)
				{
					top_chunks.push_back(header);
				}
				header = next;
				for (size_t i = top_chunks.size(); i-- > 0; )
				{
					if (top_chunks[i].offset + top_chunks[i].ChunkSize() == header.offset)
					{
						ChunkInfo closed = top_chunks[i];
						top_chunks.erase(top_chunks.begin() + i);
						text += getCloseChunkText(closed.chunk_id, top_chunks.size());
					}
				}

				//ChunkFunctions::SetProgressBar(form->toolProgressBar, static_cast<double>(100.0 / m_FileContainer.size * header.offset));
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