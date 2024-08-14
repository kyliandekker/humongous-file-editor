#include "game/compilers/Indexer.h"

#include <format>

#include "utils/abstractions.h"
#include "utils/string.h"
#include "low_level/ChunkInfo.h"
#include "project/Resource.h"
#include "game/compilers/Indexer/XMLItem.h"
#include "low_level/HumongousChunkDefinitions.h"

namespace resource_editor
{
	namespace game
	{
		std::map<std::string, size_t> m_Occurences;
		std::map<std::string, std::map<std::string, bool>> m_Childs;
		FILE* m_File = nullptr;

		xml::XMLItem format(chunk_reader::ChunkInfo& a_Header, const chunk_reader::FileContainer& a_FileContainer, int a_Indent = 0)
		{
			std::string chunk_id_name = std::string(reinterpret_cast<char*>(a_Header.chunk_id));
			chunk_id_name.resize(CHUNK_ID_SIZE);

			m_Occurences[chunk_id_name]++;
			if (m_Childs.find(chunk_id_name) == m_Childs.end())
			{
				m_Childs[chunk_id_name] = {};
			}

			xml::XMLItem item;
			item.m_Name = chunk_id_name;
			item.m_Indent = a_Indent;
			item.AddProperty("offset", std::to_string(a_Header.m_Offset));
			item.AddProperty("size", std::to_string(a_Header.ChunkSize()));

			chunk_reader::ChunkInfo header = a_FileContainer.GetNextChunk(a_Header.m_Offset);

			std::vector<std::string> childs = chunk_reader::SCHEMA.at(chunk_id_name);
			while (header.m_Offset < a_Header.m_Offset + a_Header.ChunkSize())
			{
				std::string child_chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
				child_chunk_id_name.resize(CHUNK_ID_SIZE);

				for (auto& schemaItem : childs)
				{
					if (schemaItem == child_chunk_id_name)
					{
						m_Childs[chunk_id_name][child_chunk_id_name] = true;
					}
				}

				item.m_Childs.push_back(format(header, a_FileContainer, a_Indent + 1));

				// TODO: This breaks if the chunk is somehow faulty. GetNextChunk keeps this in mind, but first looks for the chunk after the header.
				header = a_FileContainer.GetChunkInfo(header.m_Offset + header.ChunkSize());
			}

			return item;
		}

		void WriteXMLItemToFile(xml::XMLItem& a_Item)
		{
			std::string open_text = a_Item.SerializeOpen() + "\n";
			fwrite(open_text.c_str(), open_text.length(), 1, m_File);

			for (auto& item : a_Item.m_Childs)
			{
				WriteXMLItemToFile(item);
			}

			if (!a_Item.m_Childs.empty())
			{
				std::string close_text = a_Item.SerializeClose() + "\n";
				fwrite(close_text.c_str(), close_text.length(), 1, m_File);
			}
		}

		bool Indexer::Create(project::Resource& a_Resource)
		{
			m_Occurences.clear();
			m_Childs.clear();

			std::string path;
			const std::vector<COMDLG_FILTERSPEC> filters =
			{
				{ L"Extensible Markup Language (*.xml)", L"*.xml" }
			};

			if (!abstractions::SaveFile(path, nullptr, filters))
			{
				return false;
			}

			if (!string_extensions::ends_with(path, ".xml"))
			{
				path += ".xml";
			}

			std::vector<xml::XMLItem> items;

			chunk_reader::ChunkInfo header = a_Resource.m_FileContainer.GetChunkInfo(0);
			while (header.m_Offset < a_Resource.m_FileContainer.size())
			{
				items.push_back(format(header, a_Resource.m_FileContainer));
				header = a_Resource.m_FileContainer.GetChunkInfo(header.m_Offset + header.ChunkSize());
			}

			fopen_s(&m_File, path.c_str(), "wb");
			if (!m_File)
			{
				return false;
			}

			for (auto& chunkOccurrence : m_Occurences)
			{
				std::string chunk_text = std::format("{{'{}': {}}}", chunkOccurrence.first.c_str(), chunkOccurrence.second) + "\n";
				fwrite(chunk_text.c_str(), chunk_text.length(), 1, m_File);
			}

			fwrite("\n", 1, 1, m_File);

			std::string childChunkText;
			size_t j = 0;
			for (auto& childChunks : m_Childs)
			{
				std::string chunkText;
				size_t i = 0;
				for (auto& chunk : childChunks.second)
				{
					chunkText += std::format("'{}'{}", chunk.first, i == childChunks.second.size() - 1 ? "" : ", ");
					i++;
				}
				chunkText = std::format("{{{}}}", chunkText);

				childChunkText += std::format("'{}': {}{}", childChunks.first, chunkText, j == m_Childs.size() - 1 ? "" : ",\n");
				j++;
			}
			childChunkText = std::format("{{{}}}\n", childChunkText);

			fwrite(childChunkText.c_str(), childChunkText.size(), 1, m_File);

			for (auto& item : items)
			{
				WriteXMLItemToFile(item);
			}

			fclose(m_File);
			m_File = nullptr;
			return true;
		}
	}
}