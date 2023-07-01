#include <cstdio>
#include <cstring>
#include <string>

#include "lowlevel/HumongousChunkCollection.h"
#include "lowlevel/HumongousHeader.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/utils.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		std::string ignore_chunks[4] = 
		{ 
			LECF_CHUNK_ID, 
			LFLF_CHUNK_ID,
			TLKB_CHUNK_ID,
			SONG_CHUNK_ID
		};

		HumongousChunkCollection::HumongousChunkCollection(void* a_Ptr, size_t a_Size)
		{
			memset(a_Ptr, 0, a_Size);

			assert(a_Size != 0);
			m_Size = a_Size;
			m_Start = a_Ptr;
			m_Allocated = 0;
			m_End = utils::add(m_Start, m_Size);
		}

		HumongousChunkCollection::~HumongousChunkCollection()
		{
			free(m_Start);
			m_Start = nullptr;
		}

		HumongousHeader* HumongousChunkCollection::operator[](int a_Index)
		{
			assert(m_Start != nullptr);

			unsigned char* data = reinterpret_cast<unsigned char*>(m_Start);
			uint32_t index = 0;
			while (data < m_End)
			{
				HumongousHeader* wave_chunk_data = reinterpret_cast<HumongousHeader*>(data);

				bool skip = false;
				for (size_t i = 0; i < 2; i++)
					if (utils::chunkcmp(wave_chunk_data->chunk_id, ignore_chunks[i].c_str()) == 0)
						skip = true;

				if (!skip)
					data = reinterpret_cast<unsigned char*>(utils::add(data, sizeof(HumongousHeader) + wave_chunk_data->ChunkSize()));

				if (index == a_Index)
					return wave_chunk_data;

				index++;
			}
			return nullptr;
		}

		void* HumongousChunkCollection::Alloc(size_t a_Size)
		{
			assert(m_Start != nullptr);
			assert(m_Size > 0);
			assert(m_Allocated + a_Size <= m_Size);

			void* current = utils::add(m_Start, m_Allocated);
			m_Allocated += a_Size;
			return current;
		}

		void* HumongousChunkCollection::Alloc(size_t a_Size, const char* a_ChunkId)
		{
			HumongousHeader* header = reinterpret_cast<HumongousHeader*>(Alloc(a_Size));
			if (header)
			{
				memset(header, 0, a_Size);
				memcpy(header->chunk_id, a_ChunkId, CHUNK_ID_SIZE);
				header->SetChunkSize(static_cast<uint32_t>(a_Size));
			}
			return reinterpret_cast<void*>(header);
		}

		void* HumongousChunkCollection::End() const
		{
			return m_End;
		}

		void* HumongousChunkCollection::data() const
		{
			return m_Start;
		}

		size_t HumongousChunkCollection::GetNumberOfChunks() const
		{
			assert(m_Start != nullptr);

			unsigned char* data = reinterpret_cast<unsigned char*>(m_Start);
			size_t size = 0;
			while (data < m_End)
			{
				HumongousHeader* wave_chunk_data = reinterpret_cast<HumongousHeader*>(data);
				data = reinterpret_cast<unsigned char*>(utils::add(data, sizeof(HumongousHeader)));

				bool skip = false;
				for (size_t i = 0; i < sizeof(ignore_chunks) / sizeof(std::string); i++)
					if (utils::chunkcmp(wave_chunk_data->chunk_id, ignore_chunks[i].c_str()) == 0)
						skip = true;

				size_t chunk_size = wave_chunk_data->ChunkSize();
				if (!skip)
					data = reinterpret_cast<unsigned char*>(utils::add(data, wave_chunk_data->ChunkSize() - sizeof(HumongousHeader)));

				size++;
			}
			return size;
		}

		size_t HumongousChunkCollection::size() const
		{
			return m_Size;
		}
	}
}