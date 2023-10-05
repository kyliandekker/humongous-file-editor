#pragma once

#include <vector>

#include "base/SoundTab.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;
		struct SDAT_Chunk;
		struct HSHD_Chunk;
	}

	class TALKTab : public SoundTab
	{
	public:
		static size_t ReplaceInTalk(std::string path, chunk_reader::FileContainer*& fc, size_t offset, uaudio::wave_reader::DATA_Chunk& data_chunk, uaudio::wave_reader::FMT_Chunk& fmt_chunk);
		static bool ReplaceResource(chunk_reader::FileContainer*& fc, size_t offset);
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk);
	};
};