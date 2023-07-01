#pragma once

#include <cassert>
#include <cstdint>
#include "utils.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		/*
		 * WHAT IS THIS FILE?
		 * This is the HumongousChunkCollection class, where all the chunks are stored.
		 * It has methods to retrieve specific chunks and can remove chunks as well.
		*/

		class HumongousHeader;

		class HumongousChunkCollection
		{
		private:
			void* m_Start = nullptr;
			void* m_End = nullptr;
			size_t m_Size = 0;
			size_t m_Allocated = 0;

			void* Alloc(size_t a_Size);

			void* End() const;
		public:
			/// <summary>
			/// Allocates a chunk to the chunk allocator (if it is within the allocated space set in the constructor).
			/// </summary>
			/// <param name="a_Size">Size of the chunk that needs to be saved.</param>
			/// <param name="a_ChunkId">ID of the chunk.</param>
			/// <returns>Space reserved for the chunk.</returns>
			void* Alloc(size_t a_Size, const char* a_ChunkId);

			HumongousChunkCollection(void* a_Ptr, size_t a_Size);

			HumongousChunkCollection(const HumongousChunkCollection& rhs) = default;

			~HumongousChunkCollection();

			HumongousChunkCollection& operator=(const HumongousChunkCollection& rhs) = default;

			/// <summary>
			/// Returns the data.
			/// </summary>
			/// <returns></returns>
			void* data() const;

			/// <summary>
			/// Returns a chunk by index.
			/// </summary>
			/// <param name="a_Index">Index of the desired chunk.</param>
			/// <returns>Either a chunk or nullptr.</returns>
			HumongousHeader* operator[](int a_Index);

			/// <summary>
			/// Returns a chunk by index.
			/// </summary>
			/// <param name="a_Type">Reference to the type that needs to get the values of the chunk.</param>
			/// <param name="a_Index">Index of the desired chunk.</param>
			template <class T>
			void GetChunkFromData(T& a_Type, uint32_t a_Index) const
			{
				ChunkHeader* data = this[a_Index];
				if (data != nullptr)
				{
					a_Type = T(reinterpret_cast<T*>(utils::add(data, 0)));
					return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
				}

				return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
			}

			/// <summary>
			/// Returns the number of loaded chunks.
			/// </summary>
			/// <returns>Total number of all the chunks that are inside the chunk collection.</returns>
			size_t GetNumberOfChunks() const;

			/// <summary>
			/// Returns the total size of all chunks.
			/// </summary>
			/// <returns>Total size of all the chunks that are inside the chunk collection.</returns>
			size_t size() const;
		};
	}
}
