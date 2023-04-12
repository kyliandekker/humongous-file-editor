#pragma once

#include <string>
#include <uaudio_wave_reader/Defines.h>

namespace HumongousFileEditor
{
	namespace utils
	{
		/// <summary>
		/// Compares two chunks.
		/// </summary>
		/// <param name="chunkID1">Name of chunk 1.</param>
		/// <param name="chunkID2">Name of chunk 2.</param>
		/// <returns></returns>
		bool chunkcmp(const char* chunkID1, const char* chunkID2);

		void reverseBytes(unsigned char* start, int size);

		template <class T>
		T little_to_big_endian(unsigned char* number)
		{
			reverseBytes(number, sizeof(T));
			return *reinterpret_cast<T*>(number);;
		}

		/// <summary>
		/// Encrypts or decrypts a piece of data.
		/// </summary>
		/// <param name="data">The piece of data.</param>
		/// <param name="size">The size of the piece of data.</param>
		/// <param name="key">The XOR key.</param>
		/// <returns>Unencrypted/Encrypted data.</returns>
		unsigned char* xorShift(unsigned char* data, size_t size, char key);

		/// <summary>
		/// Adds a specified size to a pointer.
		/// </summary>
		/// <param name="a_Ptr">The pointer</param>
		/// <param name="a_Size">The size that needs to be added.</param>
		/// <returns></returns>
		void* add(void* a_Ptr, size_t a_Size);
	}
}