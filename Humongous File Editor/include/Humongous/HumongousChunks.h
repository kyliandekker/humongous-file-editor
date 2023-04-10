#pragma once

#include <uaudio_wave_reader/ChunkHeader.h>
#include <uaudio_wave_reader/Utils.h>
#include <corecrt_malloc.h>

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
		SBNG_Chunk(const SBNG_Chunk& rhs) : ChunkHeader(rhs)
		{
			if (&rhs != this && rhs.data != nullptr)
			{
				data = reinterpret_cast<unsigned char*>(malloc(rhs.chunkSize - sizeof(ChunkHeader)));
				if (data != nullptr)
				{
					memcpy(data, rhs.data, rhs.chunkSize - sizeof(ChunkHeader));
				}
			}
		}
		~SBNG_Chunk()
		{
			if (data != nullptr)
				free(data);
		}
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
		SGHD_Chunk(const SGHD_Chunk& rhs) : ChunkHeader(rhs)
		{
			if (&rhs != this)
			{
				num_of_songs = rhs.num_of_songs;
				memcpy(&unknown1, &rhs.unknown1, sizeof(unknown1));
			}
		}

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
		SGEN_Chunk(const SGEN_Chunk& rhs) : ChunkHeader(rhs)
		{
			if (&rhs != this)
			{
				id = rhs.id;
				song_pos = rhs.song_pos;
				song_size = rhs.song_size;
				unknown2 = rhs.unknown2;
			}
		}
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
		HSHD_Chunk(const HSHD_Chunk& rhs) : ChunkHeader(rhs)
		{
			if (&rhs != this)
			{
				memcpy(&unknown1, &rhs.unknown1, sizeof(unknown1));
				unknown2 = rhs.unknown2;
				unknown3 = rhs.unknown3;
				sample_rate = rhs.sample_rate;
				memcpy(&unknown4, &rhs.unknown4, sizeof(unknown4));
				memcpy(&unknown5, &rhs.unknown5, sizeof(unknown5));
				memcpy(&unknown6, &rhs.unknown6, sizeof(unknown6));
				memcpy(&unknown7, &rhs.unknown7, sizeof(unknown7));
			}
		}

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
		SDAT_Chunk(const SDAT_Chunk& rhs) : ChunkHeader(rhs)
		{
			if (&rhs != this && rhs.data != nullptr)
			{
				data = reinterpret_cast<unsigned char*>(malloc(rhs.chunkSize - sizeof(ChunkHeader)));
				if (data != nullptr)
				{
					memcpy(data, rhs.data, rhs.chunkSize - sizeof(ChunkHeader));
				}
			}
		}
		~SDAT_Chunk()
		{
			if (data != nullptr)
				free(data);
		}

		unsigned char* data = nullptr;
	};
#pragma pack(pop)
}