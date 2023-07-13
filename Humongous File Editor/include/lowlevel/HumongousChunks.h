#pragma once

#include "lowlevel/HumongousHeader.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
#pragma pack(push, 1)
		struct TLKB_Chunk : HumongousHeader
		{
			TLKB_Chunk() = default;
		};

		struct TALK_Chunk : HumongousHeader
		{
			TALK_Chunk() = default;
		};

		struct SBNG_Chunk : HumongousHeader
		{
			SBNG_Chunk() = default;
			SBNG_Chunk(const SBNG_Chunk& rhs);
			SBNG_Chunk(SBNG_Chunk* a_DataBuffer);
			~SBNG_Chunk();

			unsigned char* data = nullptr;
		};

		struct SONG_Chunk : HumongousHeader
		{
			SONG_Chunk() = default;
		};

		struct SGHD_Chunk : HumongousHeader
		{
			SGHD_Chunk() = default;
			SGHD_Chunk(SGHD_Chunk* a_DataBuffer);

			uint32_t num_of_songs = 0;
			unsigned char unknown1[28];
		};

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

		struct DIGI_Chunk : HumongousHeader
		{
			DIGI_Chunk() = default;
		};

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

		struct SDAT_Chunk : HumongousHeader
		{
			SDAT_Chunk() = default;
			SDAT_Chunk(const SDAT_Chunk& rhs);
			SDAT_Chunk(SDAT_Chunk* a_DataBuffer);
			~SDAT_Chunk();

			unsigned char* data = nullptr;
		};

		struct LECF_Chunk : HumongousHeader
		{
			LECF_Chunk() = default;
		};

		struct LFLF_Chunk : HumongousHeader
		{
			LFLF_Chunk() = default;
		};

		struct RMIM_Chunk : HumongousHeader
		{
			RMIM_Chunk() = default;
		};

		struct RMIH_CHUNK : HumongousHeader
		{
			RMIH_CHUNK() = default;
		};

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