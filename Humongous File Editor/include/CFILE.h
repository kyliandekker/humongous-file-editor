#pragma once

#include <cstdint>

namespace HumongousFileEditor
{
	public class CFILE
	{
	public:
		~CFILE();

		size_t cftell();
		void crewind();
		int cfopen_s(const char* path, const char* mode);
		void cfseek(int32_t a_Offset, int a_Origin);
		void cfread(void* a_Buffer, size_t a_ElementSize, size_t a_ElementCount);
		void revert();

		size_t lastReadSize = 0;
		size_t pos = 0;
		size_t size = 0;
		unsigned char* start = nullptr;
	};
}