#include "lowlevel/utils.h"

#include <string.h>

#include "lowlevel/Defines.h"

namespace HumongousFileEditor
{
	namespace utils
	{
		unsigned char* add(void* a_Ptr, size_t a_Size)
		{
			return reinterpret_cast<unsigned char*>(a_Ptr) + a_Size;
		}

		int getBit(char i, size_t position)
		{
			return (i & (1 << position)) >> position;
		}

		int chunkcmp(unsigned char* chunkID1, const char* chunkID2)
		{
			int b = strncmp(reinterpret_cast<const char*>(chunkID1), chunkID2, CHUNK_ID_SIZE);
			return b;
		}

		int unsignedCharCmp(unsigned char chunkID1, unsigned char chunkID2)
		{
			return chunkID1 == chunkID2;
		}

		void reverseBytes(unsigned char* start, int size)
		{
			unsigned char* lo = start;
			unsigned char* hi = start + size - 1;
			unsigned char swap;
			while (lo < hi)
			{
				swap = *lo;
				*lo++ = *hi;
				*hi-- = swap;
			}
		}

        void xorShift(unsigned char*& data, size_t size, char key)
		{
			for (int i = 0; i < size; i++)
			{
				data[i] = data[i] ^ key;
			}
		}

		bool ends_with(std::string const& value, std::string const& ending)
		{
			if (ending.size() > value.size()) return false;
			return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
		}
	}
}