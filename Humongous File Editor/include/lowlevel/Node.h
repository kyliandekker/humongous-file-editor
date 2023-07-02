#pragma once

#include <cstdint>

#include "Defines.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;

		class Node
		{
		public:
			Node() = default;
			Node(FileContainer& fileContainer, size_t offset, size_t prevOffset);
			unsigned char chunk_id[CHUNK_ID_SIZE] = {};
			unsigned char chunk_size[sizeof(uint32_t)] = {};
			size_t prev_offset = 0;
			size_t offset = 0;
			FileContainer* fileContainer = nullptr;
			bool null = true;

			/// <summary>
			/// Returns the node right after the chunk header. Will return Next() if not present.
			/// </summary>
			/// <returns></returns>
			Node Child() const;
			/// <summary>
			/// Returns the previous node.
			/// </summary>
			/// <returns></returns>
			Node Previous() const;
			/// <summary>
			/// Returns the node right after this node by adding the size.
			/// </summary>
			/// <returns></returns>
			Node Next() const;

			unsigned char* data() const;
			uint32_t ChunkSize(bool isBigEndian = true) const;
		};
	}
}