#pragma once

#include <string>
#include <vector>

#include "ChunkInfo.h"
#include "HumongousHeader.h"

namespace resource_editor
{
	namespace chunk_reader
	{
		class FileContainer
		{
		public:
			FileContainer() = default;
			FileContainer(std::string a_Path);
			FileContainer(const FileContainer& rhs);
			~FileContainer();
			
			bool Open(std::string a_Path);
			bool Unload();
			ChunkInfo GetChunkInfo(size_t a_Offset) const;
			HumongousHeader GetChunk(size_t a_Offset) const;
			ChunkInfo GetNextChunk(size_t a_Offset) const;
			ChunkInfo GetParent(size_t a_Offset) const;
			std::vector<ChunkInfo> GetChildren(size_t a_Offset) const;
			void Replace(size_t a_Offset, unsigned char* a_NewChunkData, size_t a_NewSize);
			void Decrypt(char a_Key = 0x69);

			size_t m_Size = 0;
			std::string m_Path;
			unsigned char* m_Data = nullptr;
		};
	}
}