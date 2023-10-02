#pragma once

#include "lowlevel/HumongousHeader.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
#pragma pack(push, 1)

		// This is the root chunk of a Talk file (*.HE2/*.he2).
		struct TLKB_Chunk : HumongousHeader
		{
			TLKB_Chunk() = default;
		};

		// This is a TALK chunk. It is usually found in the Talk file (*.HE2/*.he2), but can be found in the Resource file as well (*.(A)/*.(a)).
		// TALK chunks have child chunks that will always have an SDAT and HSHD chunk.
		// Occasionally, TALK chunks will have SBNG chunks.
		struct TALK_Chunk : HumongousHeader
		{
			TALK_Chunk() = default;
		};

		// This is a SBNG chunk. It is usually found inside TALK chunks as a child.
		// Currently it is unknown what the SBNG chunk is for, as the data can easily be set to 0 without any problems.
		// SBNG chunks do not always have the same size but it does not seem to be related to the SDAT chunk.
		struct SBNG_Chunk : HumongousHeader
		{
			SBNG_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// This is the root chunk of a Song file (*.HE4/*.he4).
		struct SONG_Chunk : HumongousHeader
		{
			SONG_Chunk() = default;
		};

		// This chunk appears at the start of the Song file (*.HE4/*.he4), and just describes the amount of songs that are in the file.
		struct SGHD_Chunk : HumongousHeader
		{
			SGHD_Chunk() = default;

			uint32_t num_of_songs = 0;
			unsigned char unknown1[28];
		};

		// This chunk is a pointer chunk that appears several times at the start of the Song file (*.HE4/*.he4).
		// It says where the song related to the SGEN appears and what the size is.
		// SGENs are not always in order of appearance.
		struct SGEN_Chunk : HumongousHeader
		{
			SGEN_Chunk() = default;

			uint32_t id = 0;

			// Direct pointer to the DIGI header of the song.
			uint32_t song_pos = 0;
			uint32_t song_size = 0;
			uint8_t unknown2 = 0;
		};

		// This is a DIGI chunk. DIGI chunks are found inside the Song file (*.HE4/*.he4), but can be found in the Resource file as well (*.(A)/*.(a)).
		// DIGI chunks usually will have soundtrack data, but if they are stored inside the Resource file, they will be SFX.
		// DIGI chunks have child chunks and will always have an SDAT and HSHD chunk.
		struct DIGI_Chunk : HumongousHeader
		{
			DIGI_Chunk() = default;
		};

		// This is a HSHD chunk. The chunk appears in every chunk that contains sound data.
		// It appears in TALK chunks and DIGI chunks.
		// It describes info about the sound data, such as what the sample rate is.
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
		
		// This is a SDAT chunk. This chunk appears in every chunk that contains sound information.
		// It appears in TALK chunks and DIGI chunks.
		// It contains uncompressed unsigned 8-bit PCM data.
		struct SDAT_Chunk : HumongousHeader
		{
			SDAT_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// This is the root chunk of a Resource file (*.(A)/*.(a)).
		struct LECF_Chunk : HumongousHeader
		{
			LECF_Chunk() = default;
		};

		// This is a LFLF chunk and signifies the start of a room.
		// Rooms contain scripts, sfx, costumes, backgrounds, etc.
		struct LFLF_Chunk : HumongousHeader
		{
			LFLF_Chunk() = default;
		};

		// This is a RMIM chunk. It will always be inside an LFLF chunk and contains the room background.
		// RMIM chunks have child chunks that will usually be an RMIH chunk and an IMxx chunk with usually a BMAP inside the IMxx.
		struct RMIM_Chunk : HumongousHeader
		{
			RMIM_Chunk() = default;
		};

		// This is a global script. Scripts are always found in the Resource file (*.(A)/*.(a)).
		// Scripts contain a set of bytecode instructions with arguments following each instruction (usually).
		// Instructions might for instance be "play sound from talk file with offset x).
		// Instructions might also be simple push byte, push dword instructions.
		struct SCRP_Chunk : HumongousHeader
		{
			SCRP_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// This is a global script. Scripts are always found in the Resource file (*.(A)/*.(a)).
		// Scripts contain a set of bytecode instructions with arguments following each instruction (usually).
		// Instructions might for instance be "play sound from talk file with offset x).
		// Instructions might also be simple push byte, push dword instructions.
		struct ENCD_Chunk : HumongousHeader
		{
			ENCD_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// This is a global script. Scripts are always found in the Resource file (*.(A)/*.(a)).
		// Scripts contain a set of bytecode instructions with arguments following each instruction (usually).
		// Instructions might for instance be "play sound from talk file with offset x).
		// Instructions might also be simple push byte, push dword instructions.
		struct EXCD_Chunk : HumongousHeader
		{
			EXCD_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// This is a local script. Scripts are always found in the Resource file (*.(A)/*.(a)) and usually inside Rooms (LFLFs).
		// Scripts contain a set of bytecode instructions with arguments following each instruction (usually).
		// Instructions might for instance be "play sound from talk file with offset x).
		// Instructions might also be simple push byte, push dword instructions.
		// This version of the local script is used in Humongous versions lower than 7.
		struct LSCR_Chunk : HumongousHeader
		{
			LSCR_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// This is a local script. Scripts are always found in the Resource file (*.(A)/*.(a)) and usually inside Rooms (LFLFs).
		// Scripts contain a set of bytecode instructions with arguments following each instruction (usually).
		// Instructions might for instance be "play sound from talk file with offset x).
		// Instructions might also be simple push byte, push dword instructions.
		// This version of the local script is used in Humongous version 7 and higher.
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

		// This chunk contains information about the room background such as width and height.
		// It is usually found in a RMDA chunk inside the Resource file (*.(A)/*.(a)).
		struct RMHD_Chunk : HumongousHeader
		{
			RMHD_Chunk() = default;

			uint16_t width = 0;
			uint16_t height = 0;
			uint16_t robjs = 0;
		};

		// This chunk contains information about the room images such as width and height and where on the screen they should appear.
		// It is usually found in a OBIM chunk inside the Resource file (*.(A)/*.(a)).
		// It is used for multiple images.
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

		// This chunk contains data for images.
		// It has custom encoding from Humongous entertainment and is a headache to understand and explain.
		// The first byte is the encoding and says whether it is transparent or not and what encoding it exactly is (basic, Humongous, etc).
		// The second byte is the fill color of the image.
		// BMAP chunks are usually found inside IMxx chunks in Resource files (*.(A)/*.(a)).
		struct BMAP_Chunk : HumongousHeader
		{
			BMAP_Chunk() = default;

			uint8_t encoding = 0;
			uint8_t fill_color = 0;
			unsigned char* data = nullptr;
		};

		// This chunk contains data for images.
		// It has custom encoding from Humongous entertainment and is a headache to understand and explain.
		// It is divided in strips and each strip has the same kind of setup as a BMAP chunk with encoding and fill color stored.
		// SMAP chunks are usually found inside IMxx chunks in Resource files (*.(A)/*.(a)).
		struct SMAP_Chunk : HumongousHeader
		{
			SMAP_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// This chunk is a TRNS chunk. It contains information about the room background and seems to store another fill color.
		// TRNS chunks are usually found inside RMDA chunks in Resource files (*.(A)/*.(a)).
		struct TRNS_Chunk : HumongousHeader
		{
			TRNS_Chunk() = default;

			// This seems to be the room transparency.
			uint8_t trns_val = 0;
		};

		// The DISK chunk seems to describe the number of rooms and has the number of rooms in length of data right after.
		// What this data tells me exactly is really unclear to me.
		// The DISK chunk is found inside an Index file (*.HE0/*.he0).
		struct DISK_Chunk : HumongousHeader
		{
			DISK_Chunk() = default;

			uint8_t num_rooms = 0;
			unsigned char* data = nullptr;
		};

		// The RNAM chunk contains the names for each room. It does not store the number of rooms, however.
		// I get the number of rooms from the DISK room that precedes it.
		// Each room starts with a room index (for instance room 0), and is then followed by a null-terminated string that is the name of the room.
		// After the null value, a new room follows with the same setup (index first, null-terminated string).
		// The DISK chunk is found inside an Index file (*.HE0/*.he0).
		struct RNAM_Chunk : HumongousHeader
		{
			RNAM_Chunk() = default;

			unsigned char* data = nullptr;
		};

		// The APAL chunk is a chunk that contains the palette that each room uses.
		// From my experience, it will always contain 256 colors, each with RGB (so 256 * 3).
		// The APAL chunk is found inside a Resource file (*.(A)/*.(a)).
		struct APAL_Chunk : HumongousHeader
		{
			APAL_Chunk() = default;

			// Color palette with 256 colors * rgb.
			unsigned char data[256 * 3];
		};

		// The OBIM chunk contains room images.
		// Each OBIM chunk can contain multiple images. They will always be IMxx.
		// OBIM chunks always have a IMHD chunk, but will not always contain images.
		// I think Humongous sometimes forgot to put images inside but did have the data there for them.
		// The OBIM chunk is found inside a Resource file (*.(A)/*.(a)).
		struct OBIM_Chunk : HumongousHeader
		{
			OBIM_Chunk() = default;
		};

		// The DLFL chunk describes when each room starts, not when each LFLF starts (for some reason).
		// The DLFL chunk is found inside an Index file (*.HE0/*.he0).
		struct DLFL_Chunk : HumongousHeader
		{
			DLFL_Chunk() = default;
			uint16_t num_lflfs = 0;

			// First position seems to be 0? Don't know exactly why.
			// The rest is the position of every RMIM in every LFLF. These positions are all 32bit unsigned integers.
			unsigned char* data = nullptr;
		};

		// The DIRN chunk describes each DIGI/TALK inside the Resource file (*.(A)/*.(a)).
		// The DIRN chunk is found inside an Index file (*.HE0/*.he0).
		// It starts with the number of sound files (which does not seem entirely accurate for some reason, unless I am missing some sounds).
		// Afterwards, there is a dictionary-type setup where the indexes are what LFLF it belongs to.
		// After this list of indexes, the values follow, where they are all 32-bit unsigned integers pointing to the relative position of each sound in relation to the LFLF.
		// This means that if a sound appears at byte position 6, the absolute position would be LFLF position + 6.
		struct DIRN_Chunk : HumongousHeader
		{
			DIRN_Chunk() = default;
			uint16_t num_sound_files = 0;

			// The rest consists of:
			// Indexes of the LFLF. These indexes are all 8bit unsigned integers.
			// Afterwards, the byte position from the RMIM of the LFLF. These positions are all 32bit unsigned integers.
			// Quick note: It does not seem to be in a particular order for some reason.
			// Quick note 2: Room 0 does not exist and it skips right to room 1 (even though it would be index 0 in programming).
			unsigned char* data = nullptr;
			// Quick note: First two seem to be 0,0 in this case. No idea why.
		};
#pragma pack(pop)
	}
}