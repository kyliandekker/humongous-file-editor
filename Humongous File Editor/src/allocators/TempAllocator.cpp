#include "allocators/TempAllocator.h"

#include <corecrt_malloc.h>
#include <stdio.h>
#include <cassert>
#include <vcruntime_string.h>

namespace HumongousFileEditor
{
	TempAllocator::TempAllocator(size_t size, unsigned char* data) : size(size)
	{
		assert(size > 0);
		this->data = reinterpret_cast<unsigned char*>(malloc(size));
		memcpy(this->data, data, size);
	}

	TempAllocator::TempAllocator(size_t size) : size(size)
	{
		assert(size > 0);
		this->data = reinterpret_cast<unsigned char*>(malloc(size));
	}

	TempAllocator::TempAllocator(const TempAllocator& rhs)
	{
		size = rhs.size;
		data = reinterpret_cast<unsigned char*>(malloc(size));
	}

	TempAllocator::~TempAllocator()
	{
		if (data && size > 0)
		{
			free(data);
		}
	}

	TempAllocator& TempAllocator::operator=(TempAllocator& other)
	{
		if (data)
		{
			free(data);
		}
		size = other.size;
		data = reinterpret_cast<unsigned char*>(malloc(size));
		memcpy(this->data, other.data, size);
		return *this;
	}

	TempAllocator& TempAllocator::operator=(const TempAllocator& other)
	{
		if (data)
		{
			free(data);
		}
		size = other.size;
		data = reinterpret_cast<unsigned char*>(malloc(size));
		memcpy(this->data, other.data, size);
		return *this;
	}
}