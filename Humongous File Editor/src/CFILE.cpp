#include "CFILE.h"

#include <iostream>

#include "utils/utils.h"

namespace HumongousFileEditor
{
	CFILE::~CFILE()
	{
		free(start);
	}

	size_t CFILE::cftell() const
	{
		return pos;
	}

	void CFILE::crewind()
	{
		pos = 0;
	}

	int CFILE::cfopen_s(const char* path, const char* mode)
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

	void CFILE::cfseek(int32_t a_Offset, int a_Origin)
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

	void CFILE::cfread(void* a_Buffer, size_t a_ElementSize, size_t a_ElementCount)
	{
		unsigned char* cur = reinterpret_cast<unsigned char*>(utils::add(start, pos));
		lastReadSize = a_ElementSize * a_ElementCount;
		memcpy(a_Buffer, cur, lastReadSize);
		pos += lastReadSize;
	}

	void CFILE::revert()
	{
		cfseek(-lastReadSize, SEEK_CUR);
	}
}