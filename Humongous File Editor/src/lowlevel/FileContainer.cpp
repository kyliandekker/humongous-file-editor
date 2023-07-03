#include "lowlevel/FileContainer.h"

#include <cstdio>

#include "lowlevel/utils.h"
#include "lowlevel/HumongousHeader.h"

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

		Node FileContainer::Start()
		{
			return NodeFromOffset(0);
		}

		Node FileContainer::NodeFromOffset(size_t offset)
		{
			uint32_t prev_offset = ReadPos(0, 0, offset);
			return Node(*this, offset, prev_offset);
		}

		uint32_t FileContainer::ReadPos(size_t pos, size_t prev_pos, size_t desiredOffset)
		{
			HumongousHeader header;
			memcpy(&header, utils::add(data, pos), sizeof(header));

			bool b = false;
			for (size_t j = 0; j < sizeof(known_chunks) / sizeof(std::string); j++)
				if (utils::chunkcmp(header.chunk_id, known_chunks[j].c_str()) == 0)
				{
					b = true;
					break;
				}
			if (!b)
				return prev_pos;

			if (pos != desiredOffset && desiredOffset < pos + header.ChunkSize())
				pos = ReadPos(pos + sizeof(HumongousHeader), pos, desiredOffset);

			return prev_pos;
		}

		uint32_t FileContainer::GetNext(size_t pos, size_t prev_pos, size_t desiredOffset)
		{
			HumongousHeader header;
			memcpy(&header, utils::add(data, pos + sizeof(HumongousHeader)), sizeof(header));

			bool b = false;
			for (size_t j = 0; j < sizeof(known_chunks) / sizeof(std::string); j++)
				if (utils::chunkcmp(header.chunk_id, known_chunks[j].c_str()) == 0)
				{
					b = true;
					break;
				}
			if (!b)
				return desiredOffset;

			return pos + sizeof(header);
		}
	}
}