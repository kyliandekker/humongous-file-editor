#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "ChunkInfo.h"
#include "HumongousHeader.h"
#include "low_level/Utils.h"

namespace resource_editor
{
	namespace chunk_reader
	{
		class FileContainer
		{
		public:
			FileContainer() = default;
			FileContainer(const FileContainer& rhs);
			~FileContainer();
			
			bool Open(std::string a_Path);
			bool Unload();
			ChunkInfo GetChunkInfo(size_t a_Offset) const;
			HumongousHeader GetChunk(size_t a_Offset) const;
			template <typename T>
			void GetChunk(T& a_Header, size_t a_Offset, size_t a_Size = sizeof(T)) const
			{
				memcpy(&a_Header, low_level::utils::add(m_Data, a_Offset), a_Size);
			}
			ChunkInfo GetNextChunk(size_t a_Offset) const;
			ChunkInfo GetParent(size_t a_Offset) const;

			std::vector<ChunkInfo> GetChildren(size_t a_Offset) const;
			void Replace(size_t a_Offset, void* a_NewChunkData, size_t a_NewSize);

			void Decrypt(char a_Key = 0x69);

			size_t m_Size = 0;
			std::string m_Path;
			void* m_Data = nullptr;
		};
	}
}