#include "low_level/FileContainer.h"

#include "system/Logger.h"
#include "low_level/Utils.h"
#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/HumongousChunks.h"

namespace resource_editor
{
	namespace chunk_reader
	{
		FileContainer::FileContainer(const FileContainer& rhs)
		{
			m_Size = rhs.m_Size;
			m_Path = rhs.m_Path;
			m_Data = reinterpret_cast<unsigned char*>(malloc(m_Size));
			if (m_Data != nullptr)
			{
				memcpy(m_Data, rhs.m_Data, m_Size);
			}
		}

		FileContainer::~FileContainer()
		{
			if (m_Data)
			{
				free(m_Data);
				m_Data = nullptr;
			}
		}

		bool FileContainer::Open(std::string a_Path)
		{
			FILE* file = nullptr;

			fopen_s(&file, a_Path.c_str(), "rb");
			if (file == nullptr)
			{
				LOGF(logger::LOGSEVERITY_ERROR, "Cannot open file \"%s\".", a_Path.c_str());
				return false;
			}

			fseek(file, 0, SEEK_END);
			m_Size = ftell(file);
			rewind(file);

			m_Path = a_Path;

			m_Data = reinterpret_cast<unsigned char*>(malloc(m_Size));
			if (m_Data != nullptr)
			{
				memset(m_Data, 0, m_Size);
				fread(m_Data, m_Size, 1, file);
			}
			fclose(file);
			LOGF(logger::LOGSEVERITY_INFO, "Successfully opened file \"%s\".", m_Path.c_str());

			return true;
		}

        bool FileContainer::Unload()
        {
			if (m_Data)
			{
				free(m_Data);
				m_Data = nullptr;
			}
			m_Size = 0;

            return true;
        }

		ChunkInfo FileContainer::GetChunkInfo(size_t a_Offset) const
		{
			ChunkInfo header;
			memcpy(&header, low_level::utils::add(m_Data, a_Offset), sizeof(HumongousHeader));
			header.m_Offset = static_cast<int32_t>(a_Offset);
			return header;
		}

		HumongousHeader FileContainer::GetChunk(size_t a_Offset) const
		{
			HumongousHeader header;
			memcpy(&header, low_level::utils::add(m_Data, a_Offset), sizeof(HumongousHeader));
			return header;
		}

		ChunkInfo FileContainer::GetNextChunk(size_t a_Offset) const
		{
			size_t extra_offset = 0;
			while (reinterpret_cast<unsigned char*>(low_level::utils::add(m_Data, a_Offset + extra_offset))[0] == 128)
			{
				extra_offset++;
			}

			a_Offset += extra_offset;

			ChunkInfo header = GetChunkInfo(a_Offset);

			std::string chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
			chunk_id_name.resize(CHUNK_ID_SIZE);

			std::vector<std::string> childs = SCHEMA.at(chunk_id_name);
			if (childs.size() == 0)
			{
				return GetChunkInfo(a_Offset + header.ChunkSize());
			}

			ChunkInfo next = GetChunkInfo(a_Offset + sizeof(HumongousHeader));
			for (auto& child : childs)
			{
				if (low_level::utils::chunkcmp(next.chunk_id, child.c_str()) == 0)
				{
					return next;
				}
			}

			LOG(logger::LOGSEVERITY_ASSERT, "Reached invalid chunk. Cannot go further.");
			return next;
		}

		ChunkInfo FileContainer::GetParent(size_t a_Offset) const
		{
			ChunkInfo parent;

			ChunkInfo chunk = GetChunkInfo(a_Offset);
			ChunkInfo next_chunk = GetChunkInfo(0);
			while (next_chunk.m_Offset < chunk.m_Offset)
			{
				if (next_chunk.m_Offset + next_chunk.ChunkSize() >= chunk.m_Offset + chunk.ChunkSize())
				{
					if (parent.m_Offset < next_chunk.m_Offset)
					{
						parent = next_chunk;
					}
				}
				next_chunk = GetNextChunk(next_chunk.m_Offset);
			}

			return parent;
		}

		std::vector<ChunkInfo> FileContainer::GetChildren(size_t a_Offset) const
		{
			std::vector<ChunkInfo> children;

			ChunkInfo chunk = GetChunkInfo(a_Offset);
			ChunkInfo next_chunk = GetChunkInfo(a_Offset);
			next_chunk = GetNextChunk(next_chunk.m_Offset);
			while (next_chunk.m_Offset < chunk.m_Offset + static_cast<int32_t>(chunk.ChunkSize()))
			{
				children.push_back(next_chunk);
				next_chunk = GetNextChunk(next_chunk.m_Offset);
			}

			return children;
		}

		void FileContainer::Replace(size_t a_Offset, void* a_NewChunkData, size_t a_NewSize)
		{
			ChunkInfo chunk = GetChunkInfo(a_Offset);
			const int32_t dif_size = static_cast<int32_t>(a_NewSize - chunk.ChunkSize());
			
			ChunkInfo next_chunk = GetChunkInfo(0);
			if (dif_size != 0)
			{
				while (next_chunk.m_Offset < chunk.m_Offset)
				{
					if (next_chunk.m_Offset + next_chunk.ChunkSize() >= chunk.m_Offset + chunk.ChunkSize())
					{
						HumongousHeader* header = reinterpret_cast<SGEN_Chunk*>(low_level::utils::add(m_Data, next_chunk.m_Offset));
						header->SetChunkSize(header->ChunkSize() + dif_size);
					}
					next_chunk = GetNextChunk(next_chunk.m_Offset);
				}
			}

			const size_t new_size = m_Size + dif_size;
			void* new_data = malloc(new_size);
			memset(new_data, 0, new_size);
			memcpy(new_data, m_Data, a_Offset);
			memcpy(low_level::utils::add(new_data, a_Offset), a_NewChunkData, a_NewSize);
			memcpy(low_level::utils::add(new_data, a_Offset + a_NewSize), low_level::utils::add(m_Data, a_Offset + chunk.ChunkSize()), m_Size - (a_Offset + chunk.ChunkSize()));

			free(m_Data);
			m_Data = new_data;
			m_Size += dif_size;
		}

		void FileContainer::Decrypt(char a_Key)
		{
			low_level::utils::xorShift(reinterpret_cast<unsigned char*&>(m_Data), m_Size, a_Key);
		}
	}
}