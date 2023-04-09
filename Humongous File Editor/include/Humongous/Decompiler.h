#pragma once

#include <string>

#include "Humongous/HumongousChunks.h"

namespace hfe
{
	namespace decompiler
	{
		void getChunk(uaudio::wave_reader::ChunkHeader& chunk, FILE* file);
		SGEN_Chunk getSGENChunk(FILE* file);
		DIGI_Chunk getDIGIChunk(FILE* file);
		SGHD_Chunk getSGHDChunk(FILE* file);
		HSHD_Chunk getHSHDChunk(FILE* file);
		SDAT_Chunk getSDATChunk(FILE* file);
		int decompile(std::string path);
	}
}