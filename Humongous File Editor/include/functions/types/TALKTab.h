#pragma once

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
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk);
	};
};