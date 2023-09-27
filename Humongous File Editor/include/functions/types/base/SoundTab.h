#pragma once

#include <string>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		struct HSHD_Chunk;
		struct SDAT_Chunk;
	}

	class SoundTab
	{
	public:
		static bool ReplaceResource(std::string& file_path);
		static bool SaveSound(chunk_reader::HSHD_Chunk& hshd_chunk, chunk_reader::SDAT_Chunk& sdat_chunk);
	};
};