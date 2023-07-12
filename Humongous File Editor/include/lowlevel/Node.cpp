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
		Node::Node(const Node& rhs)
		{
			memcpy(chunk_id, rhs.chunk_id, CHUNK_ID_SIZE);
			memcpy(chunk_size, rhs.chunk_size, CHUNK_ID_SIZE);
			offset = rhs.offset;
			fileContainer = rhs.fileContainer;
			null = rhs.null;
		}

		Node::Node(size_t offset)
		{
			null = true;
			this->offset = offset;
		}

		Node::Node(FileContainer& fileContainer, size_t offset)
		{
			this->fileContainer = &fileContainer;
			this->offset = offset;
			null = false;
			memcpy(chunk_id, reinterpret_cast<unsigned char*>(utils::add(this->fileContainer->data, offset)), CHUNK_ID_SIZE);
			memcpy(chunk_size, reinterpret_cast<unsigned char*>(utils::add(this->fileContainer->data, offset + CHUNK_ID_SIZE)), sizeof(uint32_t));
		}

		Node Node::Next() const
		{
			return fileContainer->GetNext(*this);
		}

		Node Node::Child() const
		{
			return fileContainer->GetChild(*this);
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