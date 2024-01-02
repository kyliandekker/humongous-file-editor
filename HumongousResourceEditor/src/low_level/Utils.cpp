#include "low_level/Utils.h"

#include <cstring>

#include "low_level/Defines.h"

namespace resource_editor
{
	namespace low_level
	{
		namespace utils
		{
			unsigned char* add(void* a_Ptr, size_t a_Size)
			{
				return reinterpret_cast<unsigned char*>(a_Ptr) + a_Size;
			}

			int getBit(char i, size_t a_Position)
			{
				return (i & (1 << a_Position)) >> a_Position;
			}

			int chunkcmp(unsigned char* a_ChunkID1, const char* a_ChunkID2)
			{
				int b = strncmp(reinterpret_cast<const char*>(a_ChunkID1), a_ChunkID2, CHUNK_ID_SIZE);
				return b;
			}

			int unsignedCharCmp(unsigned char a_ChunkID1, unsigned char a_ChunkID2)
			{
				return a_ChunkID1 == a_ChunkID2;
			}

			void reverseBytes(unsigned char* a_Start, int a_Size)
			{
				unsigned char* lo = a_Start;
				unsigned char* hi = a_Start + a_Size - 1;
				unsigned char swap;
				while (lo < hi)
				{
					swap = *lo;
					*lo++ = *hi;
					*hi-- = swap;
				}
			}

			void xorShift(unsigned char*& a_Data, size_t a_Size, char a_Key)
			{
				for (int i = 0; i < a_Size; i++)
				{
					a_Data[i] = a_Data[i] ^ a_Key;
				}
			}
		}
	}
}
