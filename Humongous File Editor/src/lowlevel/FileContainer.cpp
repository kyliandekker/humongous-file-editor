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
			size_t pos = 0, prevOffset = pos;
			while (pos < offset)
			{
				prevOffset = pos;
				unsigned char chunk_size[sizeof(uint32_t)] = {};
				memcpy(chunk_size, reinterpret_cast<unsigned char*>(utils::add(data, pos + CHUNK_ID_SIZE)), sizeof(uint32_t));
				pos += utils::reverseBytesC<uint32_t>(chunk_size);

				if (pos > offset)
				{
					pos = prevOffset + sizeof(HumongousHeader);
					if (pos == offset)
					{
						prevOffset = 0;
						break;
					}
				}
			}
			return Node(*this, offset, prevOffset);
		}
	}
}