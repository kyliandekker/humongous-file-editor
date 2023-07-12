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

			// If this is the last node, return a node with just an offset but set to null.
			if (pos == size)
				return Node(pos);

			// Read header.
			HumongousHeader header;
			memcpy(&header, utils::add(data, pos), sizeof(header));

			// Check if the header contains a known chunk id (just checking for letters does not work because of pieces of data containing letters).
			for (size_t j = 0; j < sizeof(known_chunks) / sizeof(std::string); j++)
				if (utils::chunkcmp(header.chunk_id, known_chunks[j].c_str()) == 0)
				{
					return NodeFromOffset(pos);
					break;
				}

			// Sometimes there may be a faulty chunk that contains 0x80 data for some reason. Skip it if we come across it.
			size_t extra_offset = 0;
			while (reinterpret_cast<unsigned char*>(utils::add(data, pos + extra_offset))[0] == 128)
				extra_offset++;

			Node e2 = NodeFromOffset(pos + extra_offset);
			return e2;
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