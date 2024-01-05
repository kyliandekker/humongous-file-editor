#include "utils/TempAllocator.h"

#include <corecrt_malloc.h>
#include <stdio.h>
#include <cassert>
#include <vcruntime_string.h>

namespace resource_editor
{
	TempAllocator::TempAllocator(size_t a_Size, unsigned char* a_Data) : m_Size(a_Size)
	{
		assert(a_Size > 0);
		this->m_Data = reinterpret_cast<unsigned char*>(malloc(a_Size));
		memcpy(this->m_Data, a_Data, a_Size);
	}

	TempAllocator::TempAllocator(size_t a_Size) : m_Size(a_Size)
	{
		assert(a_Size > 0);
		this->m_Data = reinterpret_cast<unsigned char*>(malloc(a_Size));
	}

	TempAllocator::TempAllocator(const TempAllocator& rhs)
	{
		m_Size = rhs.m_Size;
		m_Data = reinterpret_cast<unsigned char*>(malloc(m_Size));
	}

	TempAllocator::~TempAllocator()
	{
		if (m_Data && m_Size > 0)
		{
			free(m_Data);
		}
	}

	TempAllocator& TempAllocator::operator=(TempAllocator& other)
	{
		if (m_Data)
		{
			free(m_Data);
		}
		m_Size = other.m_Size;
		m_Data = reinterpret_cast<unsigned char*>(malloc(m_Size));
		memcpy(this->m_Data, other.m_Data, m_Size);
		return *this;
	}

	TempAllocator& TempAllocator::operator=(const TempAllocator& other)
	{
		if (m_Data)
		{
			free(m_Data);
		}
		m_Size = other.m_Size;
		m_Data = reinterpret_cast<unsigned char*>(malloc(m_Size));
		memcpy(this->m_Data, other.m_Data, m_Size);
		return *this;
	}
}