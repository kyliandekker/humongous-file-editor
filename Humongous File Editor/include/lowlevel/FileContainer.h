#pragma once

#include <string>
#include "Node.h"
#include "HumongousChunks.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer
		{
		public:
			FileContainer(std::string path);
			~FileContainer();

			Node Start();
			Node NodeFromOffset(size_t offset);

			Node GetNext(const Node& node);
			Node GetChild(const Node& node);

			size_t size = 0;
			unsigned char* data = nullptr;

			std::string known_chunks[125]
			{
				TLKB_CHUNK_ID,
				TALK_CHUNK_ID,
				SBNG_CHUNK_ID,
				SONG_CHUNK_ID,
				SGHD_CHUNK_ID,
				SGEN_CHUNK_ID,
				DIGI_CHUNK_ID,
				HSHD_CHUNK_ID,
				SDAT_CHUNK_ID,
				LECF_CHUNK_ID,
				LFLF_CHUNK_ID,
				RMIM_CHUNK_ID,
				RMIH_CHUNK_ID,
				SCRP_CHUNK_ID,
				SMAP_CHUNK_ID,
				BMAP_CHUNK_ID,
				BOMP_CHUNK_ID,
				LOFF_CHUNK_ID,
				RMDA_CHUNK_ID,
				ROOM_CHUNK_ID,
				RMSC_CHUNK_ID,
				SOUN_CHUNK_ID,
				AKOS_CHUNK_ID,
				COST_CHUNK_ID,
				CHAR_CHUNK_ID,
				MULT_CHUNK_ID,
				AWIZ_CHUNK_ID,
				TLKE_CHUNK_ID,
				RMHD_CHUNK_ID,
				CYCL_CHUNK_ID,
				PALS_CHUNK_ID,
				IMAG_CHUNK_ID,
				OBIM_CHUNK_ID,
				BOXD_CHUNK_ID,
				BOXM_CHUNK_ID,
				SCAL_CHUNK_ID,
				TRNS_CHUNK_ID,
				EPAL_CHUNK_ID,
				CLUT_CHUNK_ID,
				OBCD_CHUNK_ID,
				EXCD_CHUNK_ID,
				ENCD_CHUNK_ID,
				NLSC_CHUNK_ID,
				LSCR_CHUNK_ID,
				LSC2_CHUNK_ID,
				POLD_CHUNK_ID,
				OFFS_CHUNK_ID,
				APAL_CHUNK_ID,
				SEQI_CHUNK_ID,
				ZP01_CHUNK_ID,
				ZP02_CHUNK_ID,
				ZP03_CHUNK_ID,
				ZP04_CHUNK_ID,
				ZP05_CHUNK_ID,
				CDHD_CHUNK_ID,
				OBNA_CHUNK_ID,
				VERB_CHUNK_ID,
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
				AKHD_CHUNK_ID,
				AKPL_CHUNK_ID,
				AKSQ_CHUNK_ID,
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
				RNAM_CHUNK_ID,
				MAXS_CHUNK_ID,
				DROO_CHUNK_ID,
				DSCR_CHUNK_ID,
				DSOU_CHUNK_ID,
				DCOS_CHUNK_ID,
				DCHR_CHUNK_ID,
				DOBJ_CHUNK_ID,
				RGBS_CHUNK_ID,
				AKCH_CHUNK_ID,
				CNVS_CHUNK_ID,
				WIZH_CHUNK_ID,
				WIZD_CHUNK_ID,
				SPOT_CHUNK_ID,
				RELO_CHUNK_ID,
				TEXT_CHUNK_ID,
				NAME_CHUNK_ID,
				STOF_CHUNK_ID,
				SQLC_CHUNK_ID,
				SIZE_CHUNK_ID,
				DIRI_CHUNK_ID,
				DIRR_CHUNK_ID,
				DIRS_CHUNK_ID,
				DIRN_CHUNK_ID,
				DIRC_CHUNK_ID,
				DIRF_CHUNK_ID,
				DIRM_CHUNK_ID,
				DIRT_CHUNK_ID,
				DLFL_CHUNK_ID,
				DISK_CHUNK_ID,
				SVER_CHUNK_ID,
				AARY_CHUNK_ID,
				INIB_CHUNK_ID,
				NOTE_CHUNK_ID,
				ANAM_CHUNK_ID,
			};
		};
	}
}