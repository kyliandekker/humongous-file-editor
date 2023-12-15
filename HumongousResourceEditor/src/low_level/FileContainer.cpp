#include "low_level/FileContainer.h"

#include "system/Logger.h"
#include "low_level/Utils.h"
#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/HumongousChunks.h"

namespace resource_editor
{
	namespace chunk_reader
	{
		FileContainer::FileContainer(std::string path)
		{
			Open(path);
		}

		FileContainer::FileContainer(const FileContainer& rhs)
		{
			size = rhs.size;
			path = rhs.path;
			data = reinterpret_cast<unsigned char*>(malloc(size));
			if (data != nullptr)
			{
				memcpy(data, rhs.data, size);
			}
		}

		FileContainer::~FileContainer()
		{
			if (data)
			{
				free(data);
				data = nullptr;
			}
		}

		bool FileContainer::Open(std::string path)
		{
			FILE* file = nullptr;

			fopen_s(&file, path.c_str(), "rb");
			if (file == nullptr)
			{
				LOGF(logger::LOGSEVERITY_ERROR, "Cannot open file \"%s\".", path.c_str());
				return false;
			}

			fseek(file, 0, SEEK_END);
			size = ftell(file);
			rewind(file);

			this->path = path;

			data = reinterpret_cast<unsigned char*>(malloc(size));
			if (data != nullptr)
			{
				memset(data, 0, size);
				fread(data, size, 1, file);
			}
			fclose(file);
			LOGF(logger::LOGSEVERITY_INFO, "Successfully opened file \"%s\".", path.c_str());

			return true;
		}

        bool FileContainer::Unload()
        {
			if (data)
			{
				free(data);
				data = nullptr;
			}
			size = 0;

            return true;
        }

		ChunkInfo FileContainer::GetChunkInfo(size_t offset) const
		{
			ChunkInfo header;
			memcpy(&header, low_level::utils::add(data, offset), sizeof(HumongousHeader));
			header.offset = offset;
			return header;
		}

		HumongousHeader FileContainer::GetChunk(size_t offset) const
		{
			HumongousHeader header;
			memcpy(&header, low_level::utils::add(data, offset), sizeof(HumongousHeader));
			return header;
		}

		ChunkInfo FileContainer::GetNextChunk(size_t offset) const
		{
			size_t extra_offset = 0;
			while (low_level::utils::add(data, offset + extra_offset)[0] == 128)
			{
				extra_offset++;
			}

			offset += extra_offset;

			ChunkInfo header = GetChunkInfo(offset);

			std::string chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
			chunk_id_name.resize(CHUNK_ID_SIZE);

			std::vector<std::string> childs = SCHEMA.at(chunk_id_name);
			if (childs.size() == 0)
			{
				return GetChunkInfo(offset + header.ChunkSize());
			}

			ChunkInfo next = GetChunkInfo(offset + sizeof(HumongousHeader));
			for (size_t i = 0; i < childs.size(); i++)
			{
				if (low_level::utils::chunkcmp(next.chunk_id, childs[i].c_str()) == 0)
				{
					return next;
				}
			}

			assert(false);
			return next;
		}

		ChunkInfo FileContainer::GetParent(size_t offset) const
		{
			ChunkInfo parent;

			ChunkInfo chunk = GetChunkInfo(offset);
			ChunkInfo next_chunk = GetChunkInfo(0);
			while (next_chunk.offset < chunk.offset)
			{
				if (next_chunk.offset + next_chunk.ChunkSize() >= chunk.offset + chunk.ChunkSize())
				{
					if (parent.offset < next_chunk.offset)
					{
						parent = next_chunk;
					}
				}
				next_chunk = GetNextChunk(next_chunk.offset);
			}

			return parent;
		}

		std::vector<ChunkInfo> FileContainer::GetChildren(size_t offset) const
		{
			std::vector<ChunkInfo> children;

			ChunkInfo chunk = GetChunkInfo(offset);
			ChunkInfo next_chunk = GetChunkInfo(offset);
			next_chunk = GetNextChunk(next_chunk.offset);
			while (next_chunk.offset < chunk.offset + chunk.ChunkSize())
			{
				children.push_back(next_chunk);
				next_chunk = GetNextChunk(next_chunk.offset);
			}

			return children;
		}

		void FileContainer::Replace(size_t offset, unsigned char* new_chunk_data, size_t new_size)
		{
			ChunkInfo chunk = GetChunkInfo(offset);
			int32_t dif_size = static_cast<int32_t>(new_size - chunk.ChunkSize());

			ChunkInfo next_chunk = GetChunkInfo(0);
			if (dif_size != 0)
			{
				while (next_chunk.offset < chunk.offset)
				{
					if (next_chunk.offset + next_chunk.ChunkSize() >= chunk.offset + chunk.ChunkSize())
					{
						HumongousHeader* header = reinterpret_cast<SGEN_Chunk*>(low_level::utils::add(data, next_chunk.offset));
						header->SetChunkSize(header->ChunkSize() + dif_size);
					}
					next_chunk = GetNextChunk(next_chunk.offset);
				}
			}

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(size + dif_size));
			memset(new_data, 0, size + dif_size);
			memcpy(new_data, data, offset);
			memcpy(low_level::utils::add(new_data, offset), new_chunk_data, new_size);
			memcpy(low_level::utils::add(new_data, offset + new_size), low_level::utils::add(data, offset + chunk.ChunkSize()), size - (offset + chunk.ChunkSize()));

			free(data);
			data = new_data;
			size += dif_size;
		}

		void FileContainer::Decrypt(char key)
		{
			low_level::utils::xorShift(data, size, key);
		}
	}
}