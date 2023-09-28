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

			unsigned char* data = nullptr;
		};

		struct SONG_Chunk : HumongousHeader
		{
			SONG_Chunk() = default;
		};

		struct SGHD_Chunk : HumongousHeader
		{
			SGHD_Chunk() = default;

			uint32_t num_of_songs = 0;
			unsigned char unknown1[28];
		};

		struct SGEN_Chunk : HumongousHeader
		{
			SGEN_Chunk() = default;

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

		// Global script apparently.
		struct SCRP_Chunk : HumongousHeader
		{
			SCRP_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// Global script apparently.
		struct ENCD_Chunk : HumongousHeader
		{
			ENCD_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// Global script apparently.
		struct EXCD_Chunk : HumongousHeader
		{
			EXCD_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// Local script apparently. For versions < 7.
		struct LSCR_Chunk : HumongousHeader
		{
			LSCR_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// Local script apparently.
		struct LSC2_Chunk : HumongousHeader
		{
			LSC2_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// Verb script apparently.
		struct VERB_Chunk : HumongousHeader
		{
			VERB_Chunk() = default;

			unsigned char* data = nullptr;
		};

		struct RMHD_Chunk : HumongousHeader
		{
			RMHD_Chunk() = default;

			uint16_t width = 0;
			uint16_t height = 0;
			uint16_t robjs = 0;
		};

		struct IMHD_Chunk : HumongousHeader
		{
			IMHD_Chunk() = default;

			uint16_t id = 0;
			uint16_t num_imnn = 0;
			uint16_t nums_zpnn = 0;
			uint8_t flags = 0;
			uint8_t unknown = 0;
			uint16_t x = 0;
			uint16_t y = 0;
			uint16_t width = 0;
			uint16_t height = 0;
			unsigned char* data = nullptr;
		};

		struct BMAP_Chunk : HumongousHeader
		{
			BMAP_Chunk() = default;

			uint8_t encoding = 0;
			uint8_t fill_color = 0;
			unsigned char* data = nullptr;
		};

		struct SMAP_Chunk : HumongousHeader
		{
			SMAP_Chunk() = default;

			unsigned char* data = nullptr;
		};

		struct TRNS_Chunk : HumongousHeader
		{
			TRNS_Chunk() = default;

			// This seems to be the room transparency.
			uint8_t trns_val = 0;
		};

		struct DISK_Chunk : HumongousHeader
		{
			DISK_Chunk() = default;

			uint8_t num_rooms = 0;
			unsigned char* data = nullptr;
		};

		struct RNAM_Chunk : HumongousHeader
		{
			RNAM_Chunk() = default;

			unsigned char* data = nullptr;
		};

		struct APAL_Chunk : HumongousHeader
		{
			APAL_Chunk() = default;

			// Color palette with 256 colors * rgb.
			unsigned char data[256 * 3];
		};

		struct OBIM_Chunk : HumongousHeader
		{
			OBIM_Chunk() = default;
		};

		struct DLFL_Chunk : HumongousHeader
		{
			DLFL_Chunk() = default;
			uint16_t num_lflfs = 0;

			// The rest is the position of every RMIM in every LFLF. These positions are all 32bit unsigned integers.
			unsigned char* data = nullptr;
		};

		struct DIRN_Chunk : HumongousHeader
		{
			DIRN_Chunk() = default;
			uint16_t num_sound_files = 0;

			// The rest consists of:
			// Indexes of the LFLF. These indexes are all 8bit unsigned integers.
			// Afterwards, the byte position from the RMIM of the LFLF. These positions are all 32bit unsigned integers.
			unsigned char* data = nullptr;
			// Quick note: First two seem to be 0,0 in this case. No idea why.
		};
#pragma pack(pop)
	}
}