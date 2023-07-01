#pragma once

#include "HumongousHeader.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
#pragma pack(push, 1)
		constexpr auto TLKB_CHUNK_ID = "TLKB";
		struct TLKB_Chunk : HumongousHeader
		{
			TLKB_Chunk() = default;
		};

		constexpr auto TALK_CHUNK_ID = "TALK";
		struct TALK_Chunk : HumongousHeader
		{
			TALK_Chunk() = default;
		};

		constexpr auto SBNG_CHUNK_ID = "SBNG";
		struct SBNG_Chunk : HumongousHeader
		{
			SBNG_Chunk() = default;
			SBNG_Chunk(const SBNG_Chunk& rhs);
			SBNG_Chunk(SBNG_Chunk* a_DataBuffer);
			~SBNG_Chunk();

			unsigned char* data = nullptr;
		};

		constexpr auto SONG_CHUNK_ID = "SONG";
		struct SONG_Chunk : HumongousHeader
		{
			SONG_Chunk() = default;
		};

		constexpr auto SGHD_CHUNK_ID = "SGHD";
		struct SGHD_Chunk : HumongousHeader
		{
			SGHD_Chunk() = default;
			SGHD_Chunk(SGHD_Chunk* a_DataBuffer);

			uint32_t num_of_songs = 0;
			unsigned char unknown1[28];
		};

		constexpr auto SGEN_CHUNK_ID = "SGEN";
		struct SGEN_Chunk : HumongousHeader
		{
			SGEN_Chunk() = default;
			SGEN_Chunk(SGEN_Chunk* a_DataBuffer);

			uint32_t id = 0;
			// Direct pointer to the DIGI header of the song.
			uint32_t song_pos = 0;
			uint32_t song_size = 0;
			uint8_t unknown2 = 0;
		};

		constexpr auto DIGI_CHUNK_ID = "DIGI";
		struct DIGI_Chunk : HumongousHeader
		{
			DIGI_Chunk() = default;
		};

		constexpr auto HSHD_CHUNK_ID = "HSHD";
		struct HSHD_Chunk : HumongousHeader
		{
			HSHD_Chunk() = default;
			HSHD_Chunk(HSHD_Chunk* a_DataBuffer);

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
			uint32_t unknown6 = 6747836;
		};

		constexpr auto SDAT_CHUNK_ID = "SDAT";
		struct SDAT_Chunk : HumongousHeader
		{
			SDAT_Chunk() = default;
			SDAT_Chunk(const SDAT_Chunk& rhs);
			SDAT_Chunk(SDAT_Chunk* a_DataBuffer);
			~SDAT_Chunk();

			unsigned char* data = nullptr;
		};

		constexpr auto LECF_CHUNK_ID = "LECF";
		struct LECF_Chunk : HumongousHeader
		{
			LECF_Chunk() = default;
		};

		constexpr auto LFLF_CHUNK_ID = "LFLF";
		struct LFLF_Chunk : HumongousHeader
		{
			LFLF_Chunk() = default;
		};

		constexpr auto RMIM_CHUNK_ID = "RMIM";
		struct RMIM_Chunk : HumongousHeader
		{
			RMIM_Chunk() = default;
		};

		constexpr auto RMIH_CHUNK_ID = "RMIH";
		struct RMIH_CHUNK : HumongousHeader
		{
			RMIH_CHUNK() = default;
		};

		constexpr auto SCRP_CHUNK_ID = "SCRP";
		struct SCRP_Chunk : HumongousHeader
		{
			SCRP_Chunk() = default;
			SCRP_Chunk(const SCRP_Chunk& rhs);
			SCRP_Chunk(SCRP_Chunk* a_DataBuffer);
			~SCRP_Chunk();

			unsigned char* data = nullptr;
		};
#pragma pack(pop)
	}
}