#pragma once

#include "TempAllocator.h"
#include <cstdint>

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

namespace resource_editor
{
	class STempAllocator : public TempAllocator
	{
	public:
		STempAllocator() = default;
		STempAllocator(size_t a_Size, void* a_Data);
		STempAllocator(size_t a_Size);
		STempAllocator(const STempAllocator& rhs);

		STempAllocator& operator=(const STempAllocator& a_Other);
		STempAllocator& operator=(STempAllocator& a_Other);

		bool Read(void*& a_Data, size_t a_DataSize, size_t a_Size);
		bool Write(void* a_Data, size_t a_Size);
		bool Seek(size_t a_Offset, size_t a_Whence);
		bool Save();
		size_t Tell() const;
	protected:
		size_t m_Pos = 0;
	};

	using DataStreamContainer = STempAllocator;
}