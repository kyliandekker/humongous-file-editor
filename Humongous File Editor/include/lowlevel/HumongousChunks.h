#pragma once

#include "lowlevel/HumongousHeader.h"

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

		constexpr auto SMAP_CHUNK_ID = "SMAP";
		constexpr auto BMAP_CHUNK_ID = "BMAP";
		constexpr auto BOMP_CHUNK_ID = "BOMP";
		constexpr auto LOFF_CHUNK_ID = "LOFF";
		constexpr auto RMDA_CHUNK_ID = "RMDA";
		constexpr auto ROOM_CHUNK_ID = "ROOM";
		constexpr auto RMSC_CHUNK_ID = "RMSC";
		constexpr auto SOUN_CHUNK_ID = "SOUN";
		constexpr auto AKOS_CHUNK_ID = "AKOS";
		constexpr auto COST_CHUNK_ID = "COST";
		constexpr auto CHAR_CHUNK_ID = "CHAR";
		constexpr auto MULT_CHUNK_ID = "MULT";
		constexpr auto AWIZ_CHUNK_ID = "AWIZ";
		constexpr auto TLKE_CHUNK_ID = "TLKE";
		constexpr auto RMHD_CHUNK_ID = "RMHD";
		constexpr auto CYCL_CHUNK_ID = "CYCL";
		constexpr auto PALS_CHUNK_ID = "PALS";
		constexpr auto IMAG_CHUNK_ID = "IMAG";
		constexpr auto OBIM_CHUNK_ID = "OBIM";
		constexpr auto BOXD_CHUNK_ID = "BOXD";
		constexpr auto BOXM_CHUNK_ID = "BOXM";
		constexpr auto SCAL_CHUNK_ID = "SCAL";
		constexpr auto TRNS_CHUNK_ID = "TRNS";
		constexpr auto EPAL_CHUNK_ID = "EPAL";
		constexpr auto CLUT_CHUNK_ID = "CLUT";
		constexpr auto OBCD_CHUNK_ID = "OBCD";
		constexpr auto EXCD_CHUNK_ID = "EXCD";
		constexpr auto ENCD_CHUNK_ID = "ENCD";
		constexpr auto NLSC_CHUNK_ID = "NLSC";
		constexpr auto LSCR_CHUNK_ID = "LSCR";
		constexpr auto LSC2_CHUNK_ID = "LSC2";
		constexpr auto POLD_CHUNK_ID = "POLD";
		constexpr auto OFFS_CHUNK_ID = "OFFS";
		constexpr auto APAL_CHUNK_ID = "APAL";
		constexpr auto SEQI_CHUNK_ID = "SEQI";
		constexpr auto ZP01_CHUNK_ID = "ZP01";
		constexpr auto ZP02_CHUNK_ID = "ZP02";
		constexpr auto ZP03_CHUNK_ID = "ZP03";
		constexpr auto ZP04_CHUNK_ID = "ZP04";
		constexpr auto ZP05_CHUNK_ID = "ZP05";
		constexpr auto CDHD_CHUNK_ID = "CDHD";
		constexpr auto OBNA_CHUNK_ID = "OBNA";
		constexpr auto VERB_CHUNK_ID = "VERB";
		constexpr auto IM00_CHUNK_ID = "IM00";
		constexpr auto IM01_CHUNK_ID = "IM01";
		constexpr auto IM02_CHUNK_ID = "IM02";
		constexpr auto IM03_CHUNK_ID = "IM03";
		constexpr auto IM04_CHUNK_ID = "IM04";
		constexpr auto IM05_CHUNK_ID = "IM05";
		constexpr auto IM06_CHUNK_ID = "IM06";
		constexpr auto IM07_CHUNK_ID = "IM07";
		constexpr auto IM08_CHUNK_ID = "IM08";
		constexpr auto IM09_CHUNK_ID = "IM09";
		constexpr auto IM10_CHUNK_ID = "IM10";
		constexpr auto IM11_CHUNK_ID = "IM11";
		constexpr auto IM12_CHUNK_ID = "IM12";
		constexpr auto IM13_CHUNK_ID = "IM13";
		constexpr auto IM14_CHUNK_ID = "IM14";
		constexpr auto IM15_CHUNK_ID = "IM15";
		constexpr auto IM16_CHUNK_ID = "IM16";
		constexpr auto IM17_CHUNK_ID = "IM17";
		constexpr auto AKHD_CHUNK_ID = "AKHD";
		constexpr auto AKPL_CHUNK_ID = "AKPL";
		constexpr auto AKSQ_CHUNK_ID = "AKSQ";
		constexpr auto AKOF_CHUNK_ID = "AKOF";
		constexpr auto AKCI_CHUNK_ID = "AKCI";
		constexpr auto AKCD_CHUNK_ID = "AKCD";
		constexpr auto AKLC_CHUNK_ID = "AKLC";
		constexpr auto AKST_CHUNK_ID = "AKST";
		constexpr auto AKCT_CHUNK_ID = "AKCT";
		constexpr auto SP2C_CHUNK_ID = "SP2C";
		constexpr auto SPLF_CHUNK_ID = "SPLF";
		constexpr auto CLRS_CHUNK_ID = "CLRS";
		constexpr auto IMGL_CHUNK_ID = "IMGL";
		constexpr auto SQDB_CHUNK_ID = "SQDB";
		constexpr auto AKFO_CHUNK_ID = "AKFO";
		constexpr auto RNAM_CHUNK_ID = "RNAM";
		constexpr auto MAXS_CHUNK_ID = "MAXS";
		constexpr auto DROO_CHUNK_ID = "DROO";
		constexpr auto DSCR_CHUNK_ID = "DSCR";
		constexpr auto DSOU_CHUNK_ID = "DSOU";
		constexpr auto DCOS_CHUNK_ID = "DCOS";
		constexpr auto DCHR_CHUNK_ID = "DCHR";
		constexpr auto DOBJ_CHUNK_ID = "DOBJ";
		constexpr auto RGBS_CHUNK_ID = "RGBS";
		constexpr auto AKCH_CHUNK_ID = "AKCH";
		constexpr auto CNVS_CHUNK_ID = "CNVS";
		constexpr auto WIZH_CHUNK_ID = "WIZH";
		constexpr auto WIZD_CHUNK_ID = "WIZD";
		constexpr auto SPOT_CHUNK_ID = "SPOT";
		constexpr auto RELO_CHUNK_ID = "RELO";
		constexpr auto TEXT_CHUNK_ID = "TEXT";
		constexpr auto NAME_CHUNK_ID = "NAME";
		constexpr auto STOF_CHUNK_ID = "STOF";
		constexpr auto SQLC_CHUNK_ID = "SQLC";
		constexpr auto SIZE_CHUNK_ID = "SIZE";
		constexpr auto DIRI_CHUNK_ID = "DIRI";
		constexpr auto DIRR_CHUNK_ID = "DIRR";
		constexpr auto DIRS_CHUNK_ID = "DIRS";
		constexpr auto DIRN_CHUNK_ID = "DIRN";
		constexpr auto DIRC_CHUNK_ID = "DIRC";
		constexpr auto DIRF_CHUNK_ID = "DIRF";
		constexpr auto DIRM_CHUNK_ID = "DIRM";
		constexpr auto DIRT_CHUNK_ID = "DIRT";
		constexpr auto DLFL_CHUNK_ID = "DLFL";
		constexpr auto DISK_CHUNK_ID = "DISK";
		constexpr auto SVER_CHUNK_ID = "SVER";
		constexpr auto AARY_CHUNK_ID = "AARY";
		constexpr auto INIB_CHUNK_ID = "INIB";
		constexpr auto NOTE_CHUNK_ID = "NOTE";
		constexpr auto ANAM_CHUNK_ID = "ANAM";
#pragma pack(pop)
	}
}