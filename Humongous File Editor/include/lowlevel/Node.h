#pragma once

#include <cstdint>

#include "Defines.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class HumongousHeader;
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

			/// <summary>
			/// Returns a chunk by index.
			/// </summary>
			/// <param name="a_Type">Reference to the type that needs to get the values of the chunk.</param>
			/// <param name="a_Index">Index of the desired chunk.</param>
			template <class T>
			void GetChunkFromData(T& a_Type, uint32_t a_Index) const
			{
				HumongousHeader* dat = reinterpret_cast<HumongousHeader*>(data());
				if (dat != nullptr)
				{
					a_Type = T(reinterpret_cast<T*>(utils::add(dat, 0)));
				}
			}
		};
	}
}