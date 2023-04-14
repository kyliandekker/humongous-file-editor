#pragma once

#include <uaudio_wave_reader/ChunkHeader.h>
#include <cstdint>

namespace HumongousFileEditor
{
	namespace notes
	{
#pragma pack(push, 1)
		constexpr auto HFE_CHUNK_ID = "HFE ";
		struct HFE_Chunk : uaudio::wave_reader::ChunkHeader
		{
			HFE_Chunk() = default;
			uint32_t noteCount = 0;
		};
		constexpr auto NOTE_CHUNK_ID = "NOTE";
		struct NOTE_Chunk : uaudio::wave_reader::ChunkHeader
		{
			NOTE_Chunk() = default;
			NOTE_Chunk::NOTE_Chunk(const NOTE_Chunk& rhs);

			NOTE_Chunk::~NOTE_Chunk();

			uint32_t num = 0;
			char* data = nullptr;
		};
#pragma pack(pop)
	}
}