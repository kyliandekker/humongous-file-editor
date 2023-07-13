#include "lowlevel/FileContainer.h"

#include <cstdio>

#include "lowlevel/utils.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include <cassert>

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
				printf("Cannot open file");
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
		}

		FileContainer::~FileContainer()
		{
			free(data);
			data = nullptr;
		}

		HumongousHeader FileContainer::GetChunk(size_t offset) const
		{
			ChunkInfo header;
			memcpy(&header, utils::add(data, offset), sizeof(HumongousHeader));
			return header;
		}

		ChunkInfo FileContainer::GetNextChunk(size_t offset) const
		{
			size_t extra_offset = 0;
			while (reinterpret_cast<unsigned char*>(utils::add(data, offset + extra_offset))[0] == 128)
				extra_offset++;

			offset += extra_offset;

			ChunkInfo next;
			ChunkInfo header = GetChunk(offset);

			std::string chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
			chunk_id_name.resize(CHUNK_ID_SIZE);

			std::vector<std::string> childs = SCHEMA.at(chunk_id_name);
			if (childs.size() == 0)
			{
				next.offset = offset + header.ChunkSize();
				memcpy(&next, utils::add(data, next.offset), sizeof(HumongousHeader));
				return next;
			}

			next.offset = offset + sizeof(HumongousHeader);
			for (size_t i = 0; i < childs.size(); i++)
			{
				memcpy(&next, utils::add(data, next.offset), sizeof(HumongousHeader));
				if (utils::chunkcmp(next.chunk_id, childs[i].c_str()) == 0)
					return next;
			}

			assert(false);
			return next;
		}
	}
}