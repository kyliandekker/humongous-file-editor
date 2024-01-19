#include "utils/TempAllocator.h"

#include <corecrt_malloc.h>
#include <stdio.h>
#include <cassert>
#include <vcruntime_string.h>

namespace resource_editor
{
	TempAllocator::TempAllocator(size_t a_Size, void* a_Data) : m_Size(a_Size)
	{
		assert(a_Size > 0);
		m_Data = malloc(a_Size);
		if (!m_Data)
		{
			return;
		}
		memcpy(m_Data, a_Data, a_Size);
	}

	TempAllocator::TempAllocator(size_t a_Size) : m_Size(a_Size)
	{
		assert(a_Size > 0);
		m_Data = malloc(a_Size);
		if (m_Data)
		{
			memset(m_Data, 0, a_Size);
		}
	}

	TempAllocator::TempAllocator(const TempAllocator& rhs)
	{
		m_Size = rhs.m_Size;
		m_Data = malloc(m_Size);
		if (m_Data)
		{
			memcpy(m_Data, rhs.m_Data, m_Size);
		}
	}

	TempAllocator::~TempAllocator()
	{
		if (m_Data && m_Size > 0)
		{
			free(m_Data);
		}
	}

	TempAllocator& TempAllocator::operator=(TempAllocator& a_Other)
	{
		if (&a_Other != this)
		{
			if (m_Data)
			{
				free(m_Data);
			}
			m_Size = a_Other.m_Size;
			m_Data = reinterpret_cast<unsigned char*>(malloc(m_Size));
			if (m_Data)
			{
				memcpy(m_Data, a_Other.m_Data, m_Size);
			}
		}
		return *this;
	}

	TempAllocator& TempAllocator::operator=(const TempAllocator& a_Other)
	{
		if (&a_Other != this)
		{
			if (m_Data)
			{
				free(m_Data);
			}
			m_Size = a_Other.m_Size;
			m_Data = reinterpret_cast<unsigned char*>(malloc(m_Size));
			if (m_Data)
			{
				memcpy(m_Data, a_Other.m_Data, m_Size);
			}
		}
		return *this;
	}
}