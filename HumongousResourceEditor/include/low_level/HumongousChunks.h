#pragma once

#include "low_level/HumongousHeader.h"

namespace resource_editor
{
	namespace chunk_reader
	{
#pragma pack(push, 1)

		// This is the root chunk of a Talk file (*.HE2).
		class TLKB_Chunk : public HumongousHeader
		{ };

		// This is a TALK chunk. It is usually found in the Talk file (*.HE2), but can be found in the Resource file as well (*.(A)).
		// TALK chunks have child chunks that will always have an SDAT and HSHD chunk.
		// Occasionally, TALK chunks will have SBNG chunks.
		class TALK_Chunk : public HumongousHeader
		{ };

		// This is a SBNG chunk. It is usually found inside TALK chunks as a child.
		// Currently it is unknown what the SBNG chunk is for, as the data can easily be set to 0 without any problems.
		// SBNG chunks do not always have the same size but it does not seem to be related to the SDAT chunk.
		class SBNG_Chunk : public HumongousHeader
		{
		public:
			unsigned char* data = nullptr;
		};

		// This is the root chunk of a Song file (*.HE4).
		class SONG_Chunk : public HumongousHeader
		{ };

		// This chunk appears at the start of the Song file (*.HE4), and just describes the amount of songs that are in the file.
		class SGHD_Chunk : public HumongousHeader
		{
		public:
			uint32_t num_of_songs = 0;
			unsigned char unknown1[28];
		};

		// This chunk is a pointer chunk that appears several times at the start of the Song file (*.HE4).
		// It says where the song related to the SGEN appears and what the size is.
		// SGENs are not always in order of appearance.
		class SGEN_Chunk : public HumongousHeader
		{
		public:
			uint32_t id = 0;

			// Direct pointer to the DIGI header of the song.
			uint32_t song_pos = 0;
			uint32_t song_size = 0;
			uint8_t unknown2 = 0;
		};

		// This is a DIGI chunk. DIGI chunks are found inside the Song file (*.HE4), but can be found in the Resource file as well (*.(A)).
		// DIGI chunks usually will have soundtrack data, but if they are stored inside the Resource file, they will be SFX.
		// DIGI chunks have child chunks and will always have an SDAT and HSHD chunk.
		class DIGI_Chunk : public HumongousHeader
		{ };

		// This is a HSHD chunk. The chunk appears in every chunk that contains sound data.
		// It appears in TALK chunks and DIGI chunks.
		// It describes info about the sound data, such as what the sample rate is.
		class HSHD_Chunk : public HumongousHeader
		{
		public:
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
		class SDAT_Chunk : public HumongousHeader
		{
		public:
			void* data = nullptr;
		};

		// This is the root chunk of a Resource file (*.(A)).
		class LECF_Chunk : public HumongousHeader
		{ };

		// This is a LFLF chunk and signifies the start of a room.
		// Rooms contain scripts, sfx, costumes, backgrounds, etc.
		class LFLF_Chunk : public HumongousHeader
		{ };

		// This is a RMIM chunk. It will always be inside an LFLF chunk and contains the room background.
		// RMIM chunks have child chunks that will usually be an RMIH chunk and an IMxx chunk with usually a BMAP inside the IMxx.
		class RMIM_Chunk : public HumongousHeader
		{ };

		// This is a global script. Scripts are always found in the Resource file (*.(A)).
		// Scripts contain a set of bytecode instructions with arguments following each instruction (usually).
		// Instructions might for instance be "play sound from talk file with offset x).
		// Instructions might also be simple push byte, push dword instructions.
		class SCRP_Chunk : public HumongousHeader
		{
		public:
			unsigned char* data = nullptr;
		};

		// This is a global script. Scripts are always found in the Resource file (*.(A)).
		// Scripts contain a set of bytecode instructions with arguments following each instruction (usually).
		// Instructions might for instance be "play sound from talk file with offset x).
		// Instructions might also be simple push byte, push dword instructions.
		class ENCD_Chunk : public HumongousHeader
		{
		public:
			unsigned char* data = nullptr;
		};

		// This is a global script. Scripts are always found in the Resource file (*.(A)).
		// Scripts contain a set of bytecode instructions with arguments following each instruction (usually).
		// Instructions might for instance be "play sound from talk file with offset x).
		// Instructions might also be simple push byte, push dword instructions.
		class EXCD_Chunk : public HumongousHeader
		{
		public:
			unsigned char* data = nullptr;
		};

		// This is a local script. Scripts are always found in the Resource file (*.(A)) and usually inside Rooms (LFLFs).
		// Scripts contain a set of bytecode instructions with arguments following each instruction (usually).
		// Instructions might for instance be "play sound from talk file with offset x).
		// Instructions might also be simple push byte, push dword instructions.
		// This version of the local script is used in Humongous versions lower than 7.
		class LSCR_Chunk : public HumongousHeader
		{
		public:
			unsigned char* data = nullptr;
		};

		// This is a local script. Scripts are always found in the Resource file (*.(A)) and usually inside Rooms (LFLFs).
		// Scripts contain a set of bytecode instructions with arguments following each instruction (usually).
		// Instructions might for instance be "play sound from talk file with offset x).
		// Instructions might also be simple push byte, push dword instructions.
		// This version of the local script is used in Humongous version 7 and higher.
		class LSC2_Chunk : public HumongousHeader
		{
		public:
			unsigned char* data = nullptr;
		};

		// Verb script apparently.
		class VERB_Chunk : public HumongousHeader
		{
		public:
			unsigned char* data = nullptr;
		};

		// This chunk contains information about the room background such as width and height.
		// It is usually found in a RMDA chunk inside the Resource file (*.(A)).
		class RMHD_Chunk : public HumongousHeader
		{
		public:
			uint16_t width = 0;
			uint16_t height = 0;
			uint16_t robjs = 0;
		};

		// This chunk contains information about the room images such as width and height and where on the screen they should appear.
		// It is usually found in a OBIM chunk inside the Resource file (*.(A)).
		// It is used for multiple images.
		class IMHD_Chunk : public HumongousHeader
		{
		public:
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
		// BMAP chunks are usually found inside IMxx chunks in Resource files (*.(A)).
		class BMAP_Chunk : public HumongousHeader
		{
		public:
			uint8_t encoding = 0;
			uint8_t fill_color = 0;
			unsigned char* data = nullptr;
		};

		// This chunk contains data for images.
		// It has custom encoding from Humongous entertainment and is a headache to understand and explain.
		// It is divided in strips and each strip has the same kind of setup as a BMAP chunk with encoding and fill color stored.
		// SMAP chunks are usually found inside IMxx chunks in Resource files (*.(A)).
		class SMAP_Chunk : public HumongousHeader
		{
		public:
			unsigned char* data = nullptr;
		};

		// This chunk is a TRNS chunk. It contains information about the room background and seems to store another fill color.
		// TRNS chunks are usually found inside RMDA chunks in Resource files (*.(A)).
		class TRNS_Chunk : public HumongousHeader
		{
		public:
			// This seems to be the room transparency.
			uint8_t trns_val = 0;
		};

		// The DISK chunk seems to describe the number of rooms and has the number of rooms in length of data right after.
		// What this data tells me exactly is really unclear to me.
		// The DISK chunk is found inside an Index file (*.HE0).
		class DISK_Chunk : public HumongousHeader
		{
		public:
			uint8_t num_rooms = 0;
			unsigned char* data = nullptr;
		};

		// The RNAM chunk contains the names for each room. It does not store the number of rooms, however.
		// I get the number of rooms from the DISK room that precedes it.
		// Each room starts with a room index (for instance room 0), and is then followed by a null-terminated string that is the name of the room.
		// After the null value, a new room follows with the same setup (index first, null-terminated string).
		// The DISK chunk is found inside an Index file (*.HE0).
		class RNAM_Chunk : public HumongousHeader
		{
		public:
			void* data = nullptr;
		};

		// The APAL chunk is a chunk that contains the palette that each room uses.
		// From my experience, it will always contain 256 colors, each with RGB (so 256 * 3).
		// The APAL chunk is found inside a Resource file (*.(A)).
		class APAL_Chunk : public HumongousHeader
		{
		public:
			// Color palette with 256 colors * rgb.
			unsigned char data[256 * 3];
		};

		// The OBIM chunk contains room images.
		// Each OBIM chunk can contain multiple images. They will always be IMxx.
		// OBIM chunks always have a IMHD chunk, but will not always contain images.
		// I think Humongous sometimes forgot to put images inside but did have the data there for them.
		// The OBIM chunk is found inside a Resource file (*.(A)).
		class OBIM_Chunk : public HumongousHeader
		{ };

		// This chunk is a generic wrapper for index chunks in the Index file (*.HE0)
		// First is a 16-bit integer that says how many files have been indexed.
		// It has offsets relating to specific chunks in the Resource file (*.(A)).
		// Afterwards, there is a list of 8 bit integers that represent the room number (starting from 1).
		// After that the values follow, where they are all 32-bit unsigned integers pointing to the relative position of each chunk in relation to the RMIM in each LFLF.
		// This means that if the position is 6, the absolute position would be RMIM position + 6 if you want to know the absolute position.
		class GENERIC_HE0_Chunk : public HumongousHeader
		{
		public:
			uint16_t num_files = 0;

			// Quick note: It does not seem to be in a particular order for some reason.
			// Quick note 2: Room 0 does not exist and it skips right to room 1 (even though it would be index 0 in programming).
			unsigned char* data = nullptr;
			// Quick note: Some files might start with 0,0 as their first pair.
		};

		class DLFL_Chunk : public GENERIC_HE0_Chunk
		{ };

		// The DIRN chunk indexes all SOUND files inside the Resource file (*.(A)).
		// SOUN, TALK, and DIGI chunks.
		// The DIRN chunk is found inside an Index file (*.HE0).
		class DIRN_Chunk : public GENERIC_HE0_Chunk
		{ };

		// The DIRC chunk indexes all AKOS files inside the Resource file (*.(A)).
		// AKOS chunks.
		// The DIRC chunk is found inside an Index file (*.HE0).
		class DIRC_Chunk : public GENERIC_HE0_Chunk
		{ };

		// The DIRF chunk indexes all CHAR files inside the Resource file (*.(A)).
		// CHAR chunks.
		// The DIRF chunk is found inside an Index file (*.HE0).
		class DIRF_Chunk : public GENERIC_HE0_Chunk
		{ };

		// The DIRM chunk indexes all MULT files inside the Resource file (*.(A)).
		// MULT and AWIZ chunks.
		// The DIRM chunk is found inside an Index file (*.HE0).
		class DIRM_Chunk : public GENERIC_HE0_Chunk
		{ };

		// The DIRT chunk indexes all TLKE files inside the Resource file (*.(A)).
		// TLKE chunks.
		// The DIRT chunk is found inside an Index file (*.HE0).
		class DIRT_Chunk : public GENERIC_HE0_Chunk
		{ };

		// The DIRR chunk indexes all Room Background files inside the Resource file (*.(A)).
		// RMDA chunks.
		// The DIRR chunk is found inside an Index file (*.HE0).
		class DIRR_Chunk : public GENERIC_HE0_Chunk
		{ };

		// The DIRS chunk indexes all Script files inside the Resource file (*.(A)).
		// SCRP chunks.
		// The DIRS chunk is found inside an Index file (*.HE0).
		class DIRS_Chunk : public GENERIC_HE0_Chunk
		{ };

#pragma pack(pop)
	}
}