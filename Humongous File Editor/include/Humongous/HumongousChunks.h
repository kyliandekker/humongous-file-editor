#pragma once

#include <uaudio_wave_reader/ChunkHeader.h>

namespace HumongousFileEditor
{
#pragma pack(push, 1)
	constexpr auto TLKB_CHUNK_ID = "TLKB";
	struct TLKB_Chunk : uaudio::wave_reader::ChunkHeader
	{
		TLKB_Chunk() = default;
	};

	constexpr auto TALK_CHUNK_ID = "TALK";
	struct TALK_Chunk : uaudio::wave_reader::ChunkHeader
	{
		TALK_Chunk() = default;
	};

	constexpr auto SBNG_CHUNK_ID = "SBNG";
	struct SBNG_Chunk : uaudio::wave_reader::ChunkHeader
	{
		SBNG_Chunk() = default;
		~SBNG_Chunk();
		unsigned char* data = nullptr;
	};

	constexpr auto SONG_CHUNK_ID = "SONG";
	struct SONG_Chunk : uaudio::wave_reader::ChunkHeader
	{
		SONG_Chunk() = default;
	};

	constexpr auto SGHD_CHUNK_ID = "SGHD";
	struct SGHD_Chunk : uaudio::wave_reader::ChunkHeader
	{
		SGHD_Chunk() = default;

		uint32_t num_of_songs = 0;
		unsigned char unknown1[28];
	};

	constexpr auto SGEN_CHUNK_ID = "SGEN";
	struct SGEN_Chunk : uaudio::wave_reader::ChunkHeader
	{
		SGEN_Chunk() = default;

		uint32_t id = 0;
		// Direct pointer to the DIGI header of the song.
		uint32_t song_pos = 0;
		uint32_t song_size = 0;
		uint8_t unknown2 = 0;
	};

	constexpr auto DIGI_CHUNK_ID = "DIGI";
	struct DIGI_Chunk : uaudio::wave_reader::ChunkHeader
	{
		DIGI_Chunk() = default;
	};

	constexpr auto HSHD_CHUNK_ID = "HSHD";
	struct HSHD_Chunk : uaudio::wave_reader::ChunkHeader
	{
		HSHD_Chunk() = default;

		unsigned char unknown1[2] = {
			0,
			0
		};
		uint16_t unknown2 = 32896;
		uint16_t unknown3 = 65535;
		uint16_t sample_rate = 11025;
		unsigned char unknown4[2] = {
			0,
			0
		};
		unsigned char unknown5[2] = {
			0,
			0
		};
		unsigned char unknown6[2] = {
			0,
			0
		};
		unsigned char unknown7[2] = {
			0,
			0
		};
	};

	constexpr auto SDAT_CHUNK_ID = "SDAT";
	struct SDAT_Chunk : uaudio::wave_reader::ChunkHeader
	{
		SDAT_Chunk() = default;
		SDAT_Chunk(const SDAT_Chunk& rhs);
		~SDAT_Chunk();

		unsigned char* data = nullptr;
	};

	constexpr auto LECF_CHUNK_ID = "LECF";
	struct LECF_Chunk : uaudio::wave_reader::ChunkHeader
	{
		LECF_Chunk() = default;
	};

	constexpr auto LFLF_CHUNK_ID = "LFLF";
	struct LFLF_Chunk : uaudio::wave_reader::ChunkHeader
	{
		LFLF_Chunk() = default;
	};

	constexpr auto RMIM_CHUNK_ID = "RMIM";
	struct RMIM_Chunk : uaudio::wave_reader::ChunkHeader
	{
		RMIM_Chunk() = default;
	};

	constexpr auto RMIH_CHUNK_ID = "RMIH";
	struct RMIH_CHUNK : uaudio::wave_reader::ChunkHeader
	{
		RMIH_CHUNK() = default;
	};
#pragma pack(pop)
}