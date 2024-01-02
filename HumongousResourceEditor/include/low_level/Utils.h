#pragma once

namespace resource_editor
{
	namespace low_level
	{
		namespace utils
		{
			/// <summary>
			/// Adds a specified size to a pointer.
			/// </summary>
			/// <param name="a_Ptr">The pointer</param>
			/// <param name="a_Size">The size that needs to be added.</param>
			/// <returns></returns>
			unsigned char* add(void* a_Ptr, size_t a_Size);

			int getBit(char i, size_t a_Position);

			/// <summary>
			/// Compares two chunks.
			/// </summary>
			/// <param name="chunkID1">Name of chunk 1.</param>
			/// <param name="chunkID2">Name of chunk 2.</param>
			/// <returns></returns>
			int chunkcmp(unsigned char* a_ChunkID1, const char* a_ChunkID2);

			/// <summary>
			/// Compares two chunks.
			/// </summary>
			/// <param name="chunkID1">Name of chunk 1.</param>
			/// <param name="chunkID2">Name of chunk 2.</param>
			/// <returns></returns>
			int unsignedCharCmp(unsigned char a_ChunkID1, unsigned char a_ChunkID2);

			/// <summary>
			/// Function that reverses bytes.
			/// </summary>
			/// <param name="start">Start of the array of characters.</param>
			/// <param name="chunkID2">size of the array of characters.</param>
			/// <returns></returns>
			void reverseBytes(unsigned char* a_Start, int a_Size);

			/// <summary>
			/// Function that reverses bytes and casts to specified type.
			/// </summary>
			/// <param name="number">The data that needs to be casted.</param>
			/// <returns></returns>
			template <class T>
			inline T reverseBytesC(unsigned char* a_Number)
			{
				reverseBytes(a_Number, sizeof(T));
				return *reinterpret_cast<T*>(a_Number);
			}

			void xorShift(unsigned char*& a_Data, size_t a_Size, char a_Key);
		}
	}
}