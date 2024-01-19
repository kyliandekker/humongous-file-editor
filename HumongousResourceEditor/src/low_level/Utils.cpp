#include "low_level/Utils.h"

#include <cstring>

#include "low_level/Defines.h"
#include "project/Resource.h"

namespace resource_editor
{
	namespace low_level
	{
		namespace utils
		{
			void* add(void* a_Ptr, size_t a_Size)
			{
				return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(a_Ptr) + a_Size);
			}

			int getBit(char i, size_t a_Position)
			{
				return (i & (1 << a_Position)) >> a_Position;
			}

			int chunkcmp(unsigned char* a_ChunkID1, const char* a_ChunkID2)
			{
				int b = strncmp(reinterpret_cast<const char*>(a_ChunkID1), a_ChunkID2, CHUNK_ID_SIZE);
				return b;
			}

			int chunkcmp(unsigned char* a_ChunkID1, unsigned char* a_ChunkID2)
			{
				int b = strncmp(reinterpret_cast<const char*>(a_ChunkID1), reinterpret_cast<const char*>(a_ChunkID2), CHUNK_ID_SIZE);
				return b;
			}

			int unsignedCharCmp(unsigned char a_ChunkID1, unsigned char a_ChunkID2)
			{
				return a_ChunkID1 == a_ChunkID2;
			}

			void reverseBytes(unsigned char* a_Start, int a_Size)
			{
				unsigned char* lo = a_Start;
				unsigned char* hi = a_Start + a_Size - 1;
				unsigned char swap;
				while (lo < hi)
				{
					swap = *lo;
					*lo++ = *hi;
					*hi-- = swap;
				}
			}

			void xorShift(unsigned char*& a_Data, size_t a_Size, char a_Key)
			{
				for (int i = 0; i < a_Size; i++)
				{
					a_Data[i] = a_Data[i] ^ a_Key;
				}
			}

			size_t seekChildren(std::vector<chunk_reader::ChunkInfo>& a_Children, std::vector<chunk_reader::ChunkInfo>& a_Desired)
			{
				for (auto& chunkInfo : a_Desired)
				{
					chunkInfo.m_Offset = -1;
				}

				size_t found = 0;
				for (auto& chunkInfo : a_Desired)
				{
					for (auto& childInfo : a_Children)
					{
						if (chunkcmp(chunkInfo.chunk_id, childInfo.chunk_id) == 0)
						{
							found++;
							chunkInfo = childInfo;
						}
					}
				}

				return found;
			}

			int32_t seekChildren(project::Resource& a_File, size_t a_Size, std::vector<chunk_reader::ChunkInfo>& a_Desired)
			{
				for (auto& chunkInfo : a_Desired)
				{
					chunkInfo.m_Offset = -1;
				}

				size_t found = 0;
				chunk_reader::ChunkInfo info = a_File.m_FileContainer.GetChunkInfo(0);
				while (info.m_Offset < a_Size)
				{
					for (auto& chunkInfo : a_Desired)
					{
						if (utils::chunkcmp(chunkInfo.chunk_id, info.chunk_id) == 0)
						{
							found++;
							chunkInfo = info;
						}
					}
					info = a_File.m_FileContainer.GetNextChunk(info.m_Offset);
				}
				return found;
			}
		}
	}
}
