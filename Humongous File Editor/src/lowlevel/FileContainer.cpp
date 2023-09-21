#include "lowlevel/FileContainer.h"

#include <cstdio>
#include <cassert>

#include "lowlevel/utils.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include "systems/Logger.h"
#include <lowlevel/HumongousChunks.h>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		FileContainer::FileContainer(std::string path)
		{
			FILE* file = nullptr;

			fopen_s(&file, path.c_str(), "rb");
			if (file == nullptr)
			{
				LOGF(logger::LOGSEVERITY_ERROR, "Cannot open file \"%s\".", path.c_str());
				return;
			}

			fseek(file, 0, SEEK_END);
			size = ftell(file);
			rewind(file);

			data = reinterpret_cast<unsigned char*>(malloc(size));
			if (data != nullptr)
			{
				memset(data, 0, size);
				fread(data, size, 1, file);
			}
			fclose(file);
			LOGF(logger::LOGSEVERITY_INFO, "Successfully opened file \"%s\".", path.c_str());
		}

        FileContainer::FileContainer(const FileContainer& rhs)
        {
			fileType = rhs.fileType;
			size = rhs.size;
			path = rhs.path;
			data = reinterpret_cast<unsigned char*>(malloc(size));
			if (data != nullptr)
				memcpy(data, rhs.data, size);
        }

		FileContainer::~FileContainer()
		{
			free(data);
			data = nullptr;
		}

		ChunkInfo FileContainer::GetChunkInfo(size_t offset) const
		{
			ChunkInfo header;
			memcpy(&header, utils::add(data, offset), sizeof(HumongousHeader));
			header.offset = offset;
			return header;
		}

		ChunkInfo FileContainer::GetNextChunk(size_t offset) const
		{
			size_t extra_offset = 0;
			while (reinterpret_cast<unsigned char*>(utils::add(data, offset + extra_offset))[0] == 128)
				extra_offset++;

			offset += extra_offset;

			ChunkInfo header = GetChunkInfo(offset);

			std::string chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
			chunk_id_name.resize(CHUNK_ID_SIZE);

			std::vector<std::string> childs = SCHEMA.at(chunk_id_name);
			if (childs.size() == 0)
				return GetChunkInfo(offset + header.ChunkSize());

			ChunkInfo next = GetChunkInfo(offset + sizeof(HumongousHeader));
			for (size_t i = 0; i < childs.size(); i++)
			{
				if (utils::chunkcmp(next.chunk_id, childs[i].c_str()) == 0)
					return next;
			}

			assert(false);
			return next;
		}

		void FileContainer::Replace(size_t offset, unsigned char* new_chunk_data, size_t new_size)
		{
			ChunkInfo chunk = GetChunkInfo(offset);
			size_t dif_size = chunk.ChunkSize() - new_size;

			ChunkInfo next_chunk = GetChunkInfo(sizeof(HumongousHeader));
			while (next_chunk.offset < chunk.offset)
			{
				if (next_chunk.offset + next_chunk.ChunkSize())
					next_chunk.SetChunkSize(next_chunk.ChunkSize() + dif_size);
				next_chunk = GetNextChunk(next_chunk.offset);
			}

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(size + dif_size));

			memcpy(new_data, data, offset);
			memcpy(utils::add(new_data, offset), new_chunk_data, new_size);
			memcpy(utils::add(new_data, offset + new_size), utils::add(data, offset + chunk.ChunkSize()), size - (offset + chunk.ChunkSize()));

			// AFTER PROCESSING:
			// TODO: Maybe do this specifically for replacing a song in the replace method.
			// There is a chance this was a song chunk. If so, change the offset of the SGEN and the SGENS following (or before).
			if (utils::chunkcmp(chunk.chunk_id, DIGI_CHUNK_ID) == 0)
			{
				next_chunk = GetChunkInfo(sizeof(HumongousHeader));
				while (next_chunk.offset < size)
				{
					if (utils::chunkcmp(next_chunk.chunk_id, SGEN_CHUNK_ID) == 0)
					{
						SGEN_Chunk* sgen_chunk = reinterpret_cast<SGEN_Chunk*>(utils::add(new_data, next_chunk.offset));
						if (sgen_chunk->song_pos == offset)
						{
							sgen_chunk->song_pos = offset;
							sgen_chunk->song_size = new_size;
						}
						else
						{
							sgen_chunk->song_pos = offset += dif_size;
						}
					}
					next_chunk = GetNextChunk(next_chunk.offset);
				}
			}

			free(data);
			data = new_data;
		}

		void FileContainer::Decrypt(char key)
		{
			utils::xorShift(data, size, key);
		}
	}
}