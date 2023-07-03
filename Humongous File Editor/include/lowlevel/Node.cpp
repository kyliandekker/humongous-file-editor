#include "Node.h"

#include <cstdint>
#include <cstring>

#include "lowlevel/utils.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/HumongousHeader.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		Node::Node(FileContainer& fileContainer, size_t offset, size_t prevOffset)
		{
			this->fileContainer = &fileContainer;
			this->offset = offset;
			this->prev_offset = prevOffset;
			memcpy(chunk_id, reinterpret_cast<unsigned char*>(utils::add(this->fileContainer->data, offset)), CHUNK_ID_SIZE);
			memcpy(chunk_size, reinterpret_cast<unsigned char*>(utils::add(this->fileContainer->data, offset + CHUNK_ID_SIZE)), sizeof(uint32_t));
		}

		Node Node::Previous() const
		{
			return fileContainer->NodeFromOffset(prev_offset);
		}

		Node Node::Next() const
		{
			size_t new_offset = offset + ChunkSize();

			uint32_t prev_pos = fileContainer->ReadPos(0, 0, offset);
			uint32_t pos = fileContainer->GetNext(offset, prev_pos, new_offset);
			Node n = fileContainer->NodeFromOffset(pos);
			return n;
		}

		unsigned char* Node::data() const
		{
			return reinterpret_cast<unsigned char*>(utils::add(this->fileContainer->data, offset + sizeof(HumongousHeader)));
		}

		uint32_t Node::ChunkSize(bool isBigEndian) const
		{
			unsigned char size[sizeof(uint32_t)];
			memcpy(&size, chunk_size, sizeof(uint32_t));
			return isBigEndian ? utils::reverseBytesC<uint32_t>(size) : reinterpret_cast<uint32_t>(&size);
		}
	}
}