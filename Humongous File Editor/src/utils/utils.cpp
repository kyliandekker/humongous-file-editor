#include "utils/utils.h"

namespace HumongousFileEditor
{
	namespace utils
	{
		bool chunkcmp(const char* chunkID1, const char* chunkID2)
		{
			return (strncmp(chunkID1, chunkID2, uaudio::wave_reader::CHUNK_ID_SIZE) == 0);
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

		unsigned char* xorShift(unsigned char* data, size_t size, char key)
		{
			for (int i = 0; i < size; i++)
				data[i] = data[i] ^ key;
			return data;
		}

		void* add(void* a_Ptr, size_t a_Size)
		{
			return reinterpret_cast<unsigned char*>(a_Ptr) + a_Size;
		}
	}
}