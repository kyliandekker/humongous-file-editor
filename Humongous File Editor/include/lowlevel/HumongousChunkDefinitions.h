#pragma once

#include <map>
#include <vector>
#include <string>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		constexpr auto CHUNK_ID_SIZE = 4;

		// TALK .HE2
		constexpr auto TLKB_CHUNK_ID = "TLKB";
		constexpr auto TALK_CHUNK_ID = "TALK";
		constexpr auto HSHD_CHUNK_ID = "HSHD";
		constexpr auto SBNG_CHUNK_ID = "SBNG";

		// SONG .HE4
		constexpr auto SONG_CHUNK_ID = "SONG";
		constexpr auto SGEN_CHUNK_ID = "SGEN";
		constexpr auto DIGI_CHUNK_ID = "DIGI";
		constexpr auto SGHD_CHUNK_ID = "SGHD";

		// GENERAL CHUNKS
		constexpr auto SDAT_CHUNK_ID = "SDAT";

		// RESOURCE CHUNKS .(A)
		constexpr auto IMHD_CHUNK_ID = "IMHD";
		constexpr auto LECF_CHUNK_ID = "LECF";
		constexpr auto LFLF_CHUNK_ID = "LFLF";
		constexpr auto RMIM_CHUNK_ID = "RMIM";
		constexpr auto RMIH_CHUNK_ID = "RMIH";
		constexpr auto SCRP_CHUNK_ID = "SCRP";
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
		constexpr auto ZP00_CHUNK_ID = "ZP00";
		constexpr auto ZP01_CHUNK_ID = "ZP01";
		constexpr auto ZP02_CHUNK_ID = "ZP02";
		constexpr auto ZP03_CHUNK_ID = "ZP03";
		constexpr auto ZP04_CHUNK_ID = "ZP04";
		constexpr auto ZP05_CHUNK_ID = "ZP05";
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
		constexpr auto DROO_CHUNK_ID = "DROO";
		constexpr auto DSCR_CHUNK_ID = "DSCR";
		constexpr auto DSOU_CHUNK_ID = "DSOU";
		constexpr auto DCOS_CHUNK_ID = "DCOS";
		constexpr auto DCHR_CHUNK_ID = "DCHR";
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
		constexpr auto SVER_CHUNK_ID = "SVER";
		constexpr auto ANAM_CHUNK_ID = "ANAM";
		constexpr auto WRAP_CHUNK_ID = "WRAP";
		constexpr auto DEFA_CHUNK_ID = "DEFA";
		constexpr auto RMAP_CHUNK_ID = "RMAP";
		constexpr auto CUSE_CHUNK_ID = "CUSE";
		constexpr auto IM0A_CHUNK_ID = "IM0A";
		constexpr auto IM0B_CHUNK_ID = "IM0B";
		constexpr auto IM0C_CHUNK_ID = "IM0C";
		constexpr auto IM0D_CHUNK_ID = "IM0D";
		constexpr auto IM0E_CHUNK_ID = "IM0E";
		constexpr auto IM0F_CHUNK_ID = "IM0F";
		constexpr auto XMAP_CHUNK_ID = "XMAP";
		
		// INDEX CHUKS .HE0
		constexpr auto MAXS_CHUNK_ID = "MAXS";
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
		constexpr auto RNAM_CHUNK_ID = "RNAM";
		constexpr auto DOBJ_CHUNK_ID = "DOBJ";
		constexpr auto AARY_CHUNK_ID = "AARY";
		constexpr auto INIB_CHUNK_ID = "INIB";
		constexpr auto NOTE_CHUNK_ID = "NOTE";

		constexpr auto TMSK_CHUNK_ID = "TMSK";

#define NO_CHILD {}
#define IMXX { SMAP_CHUNK_ID, BMAP_CHUNK_ID, BOMP_CHUNK_ID, ZP00_CHUNK_ID, ZP01_CHUNK_ID, ZP02_CHUNK_ID, ZP03_CHUNK_ID, ZP04_CHUNK_ID, ZP05_CHUNK_ID }

		const std::map<std::string, std::vector<std::string>> SCHEMA =
		{
			{ LECF_CHUNK_ID, {
					LOFF_CHUNK_ID,
					LFLF_CHUNK_ID
				}
			},
			{ LOFF_CHUNK_ID, NO_CHILD },
			{ LFLF_CHUNK_ID, {
					RMIM_CHUNK_ID,
					RMDA_CHUNK_ID,
					ROOM_CHUNK_ID,
					RMSC_CHUNK_ID,
					SCRP_CHUNK_ID,
					SOUN_CHUNK_ID,
					AKOS_CHUNK_ID,
					COST_CHUNK_ID,
					CHAR_CHUNK_ID,
					DIGI_CHUNK_ID,
					MULT_CHUNK_ID,
					AWIZ_CHUNK_ID,
					TALK_CHUNK_ID,
					TLKE_CHUNK_ID,
				}
			},
			{ ROOM_CHUNK_ID, {
					RMHD_CHUNK_ID,
					CYCL_CHUNK_ID,
					PALS_CHUNK_ID,
					IMAG_CHUNK_ID,
					OBIM_CHUNK_ID,
					BOXD_CHUNK_ID,
					BOXM_CHUNK_ID,
					SCAL_CHUNK_ID,
					RMSC_CHUNK_ID,
					TRNS_CHUNK_ID,
					EPAL_CHUNK_ID,
					CLUT_CHUNK_ID,
					RMIM_CHUNK_ID,
					OBCD_CHUNK_ID,
					EXCD_CHUNK_ID,
					ENCD_CHUNK_ID,
					NLSC_CHUNK_ID,
					LSCR_CHUNK_ID,
				}
			},
			{ RMDA_CHUNK_ID, {
					RMHD_CHUNK_ID,
					CYCL_CHUNK_ID,
					TRNS_CHUNK_ID,
					PALS_CHUNK_ID,
					OBIM_CHUNK_ID,
					OBCD_CHUNK_ID,
					EXCD_CHUNK_ID,
					ENCD_CHUNK_ID,
					NLSC_CHUNK_ID,
					LSC2_CHUNK_ID,
					LSCR_CHUNK_ID,
					POLD_CHUNK_ID,
				}
			},
			{ RMHD_CHUNK_ID, NO_CHILD },
			{ RMIM_CHUNK_ID, {
					RMIH_CHUNK_ID,
					IM00_CHUNK_ID,
				}
			},
			{ TRNS_CHUNK_ID, NO_CHILD },
			{ EPAL_CHUNK_ID, NO_CHILD },
			{ CYCL_CHUNK_ID, NO_CHILD },
			{ PALS_CHUNK_ID, {
					WRAP_CHUNK_ID,
				}
			},
			{ OFFS_CHUNK_ID, NO_CHILD },
			{ APAL_CHUNK_ID, NO_CHILD },
			{ WRAP_CHUNK_ID, {
					OFFS_CHUNK_ID,
					APAL_CHUNK_ID,
					SMAP_CHUNK_ID,
					BOMP_CHUNK_ID,
					AWIZ_CHUNK_ID,
					SEQI_CHUNK_ID,
				}
			},
			{ IMAG_CHUNK_ID, {
					WRAP_CHUNK_ID,
				}
			},
			{ OBIM_CHUNK_ID, {
					IMHD_CHUNK_ID,
					IMAG_CHUNK_ID,
					IM00_CHUNK_ID,
					IM01_CHUNK_ID,
					IM02_CHUNK_ID,
					IM03_CHUNK_ID,
					IM04_CHUNK_ID,
					IM05_CHUNK_ID,
					IM06_CHUNK_ID,
					IM07_CHUNK_ID,
					IM08_CHUNK_ID,
					IM09_CHUNK_ID,
					IM10_CHUNK_ID,
					IM11_CHUNK_ID,
					IM12_CHUNK_ID,
					IM13_CHUNK_ID,
					IM14_CHUNK_ID,
					IM15_CHUNK_ID,
					IM16_CHUNK_ID,
					IM17_CHUNK_ID,
				}
			},
			{ IM00_CHUNK_ID, IMXX },
			{ IM01_CHUNK_ID, IMXX },
			{ IM02_CHUNK_ID, IMXX },
			{ IM03_CHUNK_ID, IMXX },
			{ IM04_CHUNK_ID, IMXX },
			{ IM05_CHUNK_ID, IMXX },
			{ IM06_CHUNK_ID, IMXX },
			{ IM07_CHUNK_ID, IMXX },
			{ IM08_CHUNK_ID, IMXX },
			{ IM09_CHUNK_ID, IMXX },
			{ IM10_CHUNK_ID, IMXX },
			{ IM11_CHUNK_ID, IMXX },
			{ IM12_CHUNK_ID, IMXX },
			{ IM13_CHUNK_ID, IMXX },
			{ IM14_CHUNK_ID, IMXX },
			{ IM15_CHUNK_ID, IMXX },
			{ IM16_CHUNK_ID, IMXX },
			{ IM17_CHUNK_ID, IMXX },
			{ ZP00_CHUNK_ID, NO_CHILD },
			{ ZP01_CHUNK_ID, NO_CHILD },
			{ ZP02_CHUNK_ID, NO_CHILD },
			{ ZP03_CHUNK_ID, NO_CHILD },
			{ ZP04_CHUNK_ID, NO_CHILD },
			{ ZP05_CHUNK_ID, NO_CHILD },
			{ RMSC_CHUNK_ID, {
					ENCD_CHUNK_ID,
					EXCD_CHUNK_ID,
					OBCD_CHUNK_ID,
					LSCR_CHUNK_ID,
				}
			},
			{ OBCD_CHUNK_ID, {
					CDHD_CHUNK_ID,
					OBNA_CHUNK_ID,
					VERB_CHUNK_ID,
				}
			},
			{ BOXD_CHUNK_ID, NO_CHILD },
			{ BOXM_CHUNK_ID, NO_CHILD },
			{ CLUT_CHUNK_ID, NO_CHILD },
			{ SCAL_CHUNK_ID, NO_CHILD },
			{ RMIH_CHUNK_ID, NO_CHILD },
			{ AKOS_CHUNK_ID, {
					AKHD_CHUNK_ID,
					AKPL_CHUNK_ID,
					RGBS_CHUNK_ID,
					AKSQ_CHUNK_ID,
					AKCH_CHUNK_ID,
					AKOF_CHUNK_ID,
					AKCI_CHUNK_ID,
					AKCD_CHUNK_ID,
					AKLC_CHUNK_ID,
					AKST_CHUNK_ID,
					AKCT_CHUNK_ID,
					SP2C_CHUNK_ID,
					SPLF_CHUNK_ID,
					CLRS_CHUNK_ID,
					IMGL_CHUNK_ID,
					SQDB_CHUNK_ID,
					AKFO_CHUNK_ID,
				}
			},
			{ SMAP_CHUNK_ID, NO_CHILD },
			{ IMHD_CHUNK_ID, NO_CHILD },
			{ CDHD_CHUNK_ID, NO_CHILD },
			{ VERB_CHUNK_ID, NO_CHILD },
			{ OBNA_CHUNK_ID, NO_CHILD },
			{ EXCD_CHUNK_ID, NO_CHILD },
			{ ENCD_CHUNK_ID, NO_CHILD },
			{ NLSC_CHUNK_ID, NO_CHILD },
			{ LSCR_CHUNK_ID, NO_CHILD },
			{ CHAR_CHUNK_ID, NO_CHILD },
			{ SCRP_CHUNK_ID, NO_CHILD },
			{ COST_CHUNK_ID, NO_CHILD },
			{ SOUN_CHUNK_ID, NO_CHILD },
			{ BOMP_CHUNK_ID, NO_CHILD },
			{ RNAM_CHUNK_ID, NO_CHILD },
			{ MAXS_CHUNK_ID, NO_CHILD },
			{ DROO_CHUNK_ID, NO_CHILD },
			{ DSCR_CHUNK_ID, NO_CHILD },
			{ DSOU_CHUNK_ID, NO_CHILD },
			{ DCOS_CHUNK_ID, NO_CHILD },
			{ DCHR_CHUNK_ID, NO_CHILD },
			{ DOBJ_CHUNK_ID, NO_CHILD },
			{ BMAP_CHUNK_ID, NO_CHILD },
			{ LSC2_CHUNK_ID, NO_CHILD },
			{ DIGI_CHUNK_ID, {
					HSHD_CHUNK_ID,
					SBNG_CHUNK_ID,
					SDAT_CHUNK_ID,
				}
			},
			{ HSHD_CHUNK_ID, NO_CHILD },
			{ SDAT_CHUNK_ID, NO_CHILD },
			{ AKHD_CHUNK_ID, NO_CHILD },
			{ AKPL_CHUNK_ID, NO_CHILD },
			{ RGBS_CHUNK_ID, NO_CHILD },
			{ AKSQ_CHUNK_ID, NO_CHILD },
			{ AKCH_CHUNK_ID, NO_CHILD },
			{ AKOF_CHUNK_ID, NO_CHILD },
			{ AKCI_CHUNK_ID, NO_CHILD },
			{ AKCD_CHUNK_ID, NO_CHILD },
			{ AKLC_CHUNK_ID, NO_CHILD },
			{ AKST_CHUNK_ID, NO_CHILD },
			{ AKCT_CHUNK_ID, NO_CHILD },
			{ AKFO_CHUNK_ID, NO_CHILD },
			{ RMAP_CHUNK_ID, NO_CHILD },
			{ CUSE_CHUNK_ID, NO_CHILD },
			{ XMAP_CHUNK_ID, NO_CHILD },
			{ MULT_CHUNK_ID, {
					DEFA_CHUNK_ID,
					WRAP_CHUNK_ID,
				}
			},
			{ DEFA_CHUNK_ID, {
					RGBS_CHUNK_ID,
					CNVS_CHUNK_ID,
					RMAP_CHUNK_ID,
					CUSE_CHUNK_ID,
					CUSE_CHUNK_ID,
				}
			},
			{ AWIZ_CHUNK_ID, {
					WIZH_CHUNK_ID,
					WIZD_CHUNK_ID,
					CNVS_CHUNK_ID,
					SPOT_CHUNK_ID,
					RELO_CHUNK_ID,
					RGBS_CHUNK_ID,
					XMAP_CHUNK_ID,
					TRNS_CHUNK_ID,
					TRNS_CHUNK_ID,
				}
			},
			{ TLKE_CHUNK_ID, {
					TEXT_CHUNK_ID,
				}
			},
			{ TEXT_CHUNK_ID, NO_CHILD },
			{ WIZH_CHUNK_ID, NO_CHILD },
			{ WIZD_CHUNK_ID, NO_CHILD },
			{ CNVS_CHUNK_ID, NO_CHILD },
			{ SPOT_CHUNK_ID, NO_CHILD },
			{ RELO_CHUNK_ID, NO_CHILD },
			{ POLD_CHUNK_ID, NO_CHILD },
			{ SP2C_CHUNK_ID, NO_CHILD },
			{ SPLF_CHUNK_ID, NO_CHILD },
			{ CLRS_CHUNK_ID, NO_CHILD },
			{ IMGL_CHUNK_ID, NO_CHILD },
			{ SQDB_CHUNK_ID, {
					WRAP_CHUNK_ID,
				}
			},
			{ SEQI_CHUNK_ID, {
					NAME_CHUNK_ID,
					STOF_CHUNK_ID,
					SQLC_CHUNK_ID,
					SIZE_CHUNK_ID,
				}
			},
			{ NAME_CHUNK_ID, NO_CHILD },
			{ STOF_CHUNK_ID, NO_CHILD },
			{ SQLC_CHUNK_ID, NO_CHILD },
			{ SIZE_CHUNK_ID, NO_CHILD },
			{ SBNG_CHUNK_ID, NO_CHILD },
			{ TALK_CHUNK_ID, {
					HSHD_CHUNK_ID,
					SBNG_CHUNK_ID,
					SDAT_CHUNK_ID,
				}
			},
			{ DIRI_CHUNK_ID, NO_CHILD },
			{ DIRR_CHUNK_ID, NO_CHILD },
			{ DIRS_CHUNK_ID, NO_CHILD },
			{ DIRN_CHUNK_ID, NO_CHILD },
			{ DIRC_CHUNK_ID, NO_CHILD },
			{ DIRF_CHUNK_ID, NO_CHILD },
			{ DIRM_CHUNK_ID, NO_CHILD },
			{ DIRT_CHUNK_ID, NO_CHILD },
			{ DLFL_CHUNK_ID, NO_CHILD },
			{ DISK_CHUNK_ID, NO_CHILD },
			{ SVER_CHUNK_ID, NO_CHILD },
			{ AARY_CHUNK_ID, NO_CHILD },
			{ INIB_CHUNK_ID, {
					NOTE_CHUNK_ID,
				}
			},
			{ NOTE_CHUNK_ID, NO_CHILD },
			{ TLKB_CHUNK_ID, {
					SBNG_CHUNK_ID,
					TALK_CHUNK_ID,
				}
			},
			{ SONG_CHUNK_ID, {
					SGHD_CHUNK_ID,
					SGEN_CHUNK_ID,
					DIGI_CHUNK_ID,
				}
			},
			{ SGHD_CHUNK_ID, NO_CHILD },
			{ SGEN_CHUNK_ID , NO_CHILD },
			{ ANAM_CHUNK_ID , NO_CHILD },
			{ IM0A_CHUNK_ID, {
					SMAP_CHUNK_ID,
				}
			},
			{ IM0B_CHUNK_ID, {
					SMAP_CHUNK_ID,
				}
			},
			{ IM0C_CHUNK_ID, {
					SMAP_CHUNK_ID,
				}
			},
			{ IM0D_CHUNK_ID, {
					SMAP_CHUNK_ID,
				}
			},
			{ IM0E_CHUNK_ID, {
					SMAP_CHUNK_ID,
				}
			},
			{ IM0F_CHUNK_ID, {
					SMAP_CHUNK_ID,
				}
			},
			{ TMSK_CHUNK_ID, {} },
		};
	}
}