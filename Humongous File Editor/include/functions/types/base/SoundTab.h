#pragma once

#include <string>

namespace uaudio
{
	namespace wave_reader
	{
		struct FMT_Chunk;
		struct DATA_Chunk;
	}
}

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
		static bool ReplaceResource(std::string& file_path, uaudio::wave_reader::FMT_Chunk fmt_chunk, uaudio::wave_reader::DATA_Chunk data_chunk);
		static bool SaveSound(chunk_reader::HSHD_Chunk& hshd_chunk, chunk_reader::SDAT_Chunk& sdat_chunk);
	};
};