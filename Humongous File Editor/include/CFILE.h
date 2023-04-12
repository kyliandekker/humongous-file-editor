#pragma once

#include <iostream>

#include "utils/utils.h"

namespace HumongousFileEditor
{
	public class CFILE
	{
	public:
		~CFILE()
		{
			free(start);
		}

		size_t cftell()
		{
			return pos;
		}
		void crewind()
		{
			pos = 0;
		}
		int cfopen_s(const char* path, const char* mode)
		{
			FILE* file = nullptr;

			// Open the file.
			fopen_s(&file, path, mode);
			if (file == nullptr)
			{
				std::cout << "Cannot find file \"" << path << "\"." << std::endl;
				return 1;
			}

			// Go to eof and store the size.
			fseek(file, 0, SEEK_END);
			uint32_t fileSize = ftell(file);
			rewind(file);

			size = fileSize;
			start = reinterpret_cast<unsigned char*>(malloc(size));
			if (start != nullptr)
				fread(start, fileSize, 1, file);
			pos = 0;

			fclose(file);
			return 0;
		}
		void cfseek(int32_t a_Offset, int a_Origin)
		{
			switch (a_Origin)
			{
				case SEEK_CUR:
				{
					pos += a_Offset;
					break;
				}
				case SEEK_END:
				{
					pos = size - a_Offset;
					break;
				}
				case SEEK_SET:
				{
					pos = a_Offset;
					break;
				}
				default:
					break;
			}
		}
		void cfread(void* a_Buffer, size_t a_ElementSize, size_t a_ElementCount)
		{
			unsigned char* cur = reinterpret_cast<unsigned char*>(utils::add(start, pos));
			memcpy(a_Buffer, cur, a_ElementSize * a_ElementCount);
			pos += a_ElementSize * a_ElementCount;
		}

		size_t pos = 0;
		size_t size = 0;
		unsigned char* start = nullptr;
	};
}