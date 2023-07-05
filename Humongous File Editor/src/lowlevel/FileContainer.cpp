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
			return Node(*this, offset);
		}

		Node FileContainer::GetNext(const Node& node)
		{
			size_t pos = node.offset + node.ChunkSize();
			if (pos == size)
				return Node(pos);

			HumongousHeader header;
			memcpy(&header, utils::add(data, pos), sizeof(header));

			for (size_t j = 0; j < sizeof(known_chunks) / sizeof(std::string); j++)
				if (utils::chunkcmp(header.chunk_id, known_chunks[j].c_str()) == 0)
				{
					return NodeFromOffset(pos);
					break;
				}

			uint32_t extra_offset = sizeof(HumongousHeader);
			while (reinterpret_cast<uint8_t>(utils::add(data, node.offset + extra_offset)) == 128)
				extra_offset++;

			return NodeFromOffset(node.offset + extra_offset);
		}

		Node FileContainer::GetChild(const Node& node)
		{
			HumongousHeader header;
			memcpy(&header, utils::add(data, node.offset + sizeof(HumongousHeader)), sizeof(header));

			for (size_t j = 0; j < sizeof(known_chunks) / sizeof(std::string); j++)
				if (utils::chunkcmp(header.chunk_id, known_chunks[j].c_str()) == 0)
				{
					return NodeFromOffset(node.offset + sizeof(HumongousHeader));
					break;
				}
			return Node(node.offset);
		}
	}
}