#pragma once

#include <string>

#include "Humongous/HumongousChunks.h"

namespace HumongousFileEditor
{
	namespace decompiler
	{
		void getChunk(uaudio::wave_reader::ChunkHeader& chunk, FILE* file);
		SGEN_Chunk getSGENChunk(FILE* file);
		DIGI_Chunk getDIGIChunk(FILE* file);
		SGHD_Chunk getSGHDChunk(FILE* file);
		HSHD_Chunk getHSHDChunk(FILE* file);
		SDAT_Chunk getSDATChunk(FILE* file);
		TALK_Chunk getTALKChunk(FILE* file);
		SBNG_Chunk getSBNGChunk(FILE* file);
		int decompile(std::string path);
	}
}