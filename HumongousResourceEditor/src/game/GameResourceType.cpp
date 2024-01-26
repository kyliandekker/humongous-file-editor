#include "game/GameResourceType.h"
#include "low_level/Utils.h"
#include "low_level/HumongousChunkDefinitions.h"

namespace resource_editor
{
	namespace game
	{
		GameResourceType resource_editor::game::getResourceByChunkID(unsigned char a_ChunkID[CHUNK_ID_SIZE])
		{
			if (
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::SCRP_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::EXCD_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::ENCD_CHUNK_ID) == 0
			)
			{
				return GameResourceType::Global_Script;
			}
			else if (
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::LSCR_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::LSC2_CHUNK_ID) == 0
			)
			{
				return GameResourceType::Local_Script;
			}
			else if (low_level::utils::chunkcmp(a_ChunkID, chunk_reader::VERB_CHUNK_ID) == 0)
			{
				return GameResourceType::Verb_Script;
			}
			else if (low_level::utils::chunkcmp(a_ChunkID, chunk_reader::TALK_CHUNK_ID) == 0)
			{
				return GameResourceType::Talkie;
			}
			else if (low_level::utils::chunkcmp(a_ChunkID, chunk_reader::DIGI_CHUNK_ID) == 0)
			{
				return GameResourceType::SFX;
			}
			else if (low_level::utils::chunkcmp(a_ChunkID, chunk_reader::SGEN_CHUNK_ID) == 0)
			{
				return GameResourceType::SFX;
			}
			else if (low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM00_CHUNK_ID) == 0)
			{
				return GameResourceType::RoomBackground;
			}
			else if (
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM01_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM02_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM03_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM04_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM05_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM06_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM07_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM08_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM09_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM10_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM11_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM12_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM13_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM14_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM15_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM16_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM17_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM0A_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM0B_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM0C_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM0D_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM0E_CHUNK_ID) == 0 ||
				low_level::utils::chunkcmp(a_ChunkID, chunk_reader::IM0F_CHUNK_ID) == 0
			)
			{
				return GameResourceType::RoomImage;
			}
			return GameResourceType::Unknown;
		}
	}
}
