#pragma once

#include <string>

namespace uaudio
{
	namespace wave_reader
	{
		class ChunkHeader;
	}
}

namespace HumongousFileEditor
{
	namespace compiler
	{
		uint32_t getChunksize(uint32_t data);
		int compileHE2(std::string path);
		int compileHE4(std::string path);
		int compile(std::string path);
	}
}