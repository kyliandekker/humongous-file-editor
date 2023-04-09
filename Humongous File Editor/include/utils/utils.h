#pragma once

#include <string.h>
#include <uaudio_wave_reader/Defines.h>

namespace hfe
{
	namespace utils
	{
		/// <summary>
		/// Big endian to little endian.
		/// </summary>
		/// <param name="data">Number in char array.</param>
		/// <returns></returns>
		int big_to_little_endian(unsigned char* data)
		{
			int out = (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | ((unsigned)data[0] << 24);
			return out;
		}

		/// <summary>
		/// Compares two chunks.
		/// </summary>
		/// <param name="chunkID1">Name of chunk 1.</param>
		/// <param name="chunkID2">Name of chunk 2.</param>
		/// <returns></returns>
		bool chunkcmp(const char* chunkID1, const char* chunkID2)
		{
			return (strncmp(chunkID1, chunkID2, uaudio::wave_reader::CHUNK_ID_SIZE) == 0);
		}

		unsigned char* to_big_endian_2(unsigned char* number, size_t size)
		{
			unsigned char* big_endian = reinterpret_cast<unsigned char*>(malloc(size));
			uint32_t current = size - 1;
			for (size_t i = 0; i < size; i++, current--)
				big_endian[i] = (reinterpret_cast<uint64_t>(number) >> (8 * current)) & 0xFF;
			return big_endian;
		}
	}
}