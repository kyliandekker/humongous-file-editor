#include "allocators/ChunkCollectionWrapper.h"

#include <corecrt_malloc.h>

namespace uaudio
{
	namespace wave_reader
	{
		ChunkCollectionWrapper::ChunkCollectionWrapper(size_t a_Size) : ChunkCollection(malloc(a_Size), a_Size)
		{
		}

		ChunkCollectionWrapper::ChunkCollectionWrapper(void* a_Ptr, size_t a_Size) : ChunkCollection(a_Ptr, a_Size)
		{
		}

		ChunkCollectionWrapper::~ChunkCollectionWrapper()
		{
			if (m_Start)
			{
				free(m_Start);
				m_Start = nullptr;
				m_Size = 0;
			}
		}

		ChunkCollectionWrapper::ChunkCollectionWrapper(const ChunkCollectionWrapper& rhs)
		{
			m_Allocated = rhs.m_Allocated;
			m_Size = rhs.m_Size;
			ChunkCollection(malloc(m_Size), m_Size);
			memcpy(m_Start, rhs.m_Start, rhs.m_Size);
		}

		ChunkCollectionWrapper& ChunkCollectionWrapper::operator=(const ChunkCollectionWrapper& rhs)
		{
			if (this != &rhs)
			{
				m_Allocated = rhs.m_Allocated;
				m_Size = rhs.m_Size;

				m_Start = malloc(m_Size);
				m_End = utils::add(m_Start, m_Size);

				memcpy(m_Start, rhs.m_Start, rhs.m_Size);
			}
			return *this;
		}
	}
}