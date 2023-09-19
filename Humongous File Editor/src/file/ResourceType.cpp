#include "file/ResourceType.h"
#include "lowlevel/utils.h"
#include "lowlevel/HumongousChunkDefinitions.h"

namespace HumongousFileEditor
{
	namespace files
	{
		ResourceType GetResourceTypeByChunkName(unsigned char* chunk_id)
		{
			if (utils::chunkcmp(chunk_id, chunk_reader::SONG_CHUNK_ID) == 0)
				return ResourceType::Song;
			else if (utils::chunkcmp(chunk_id, chunk_reader::SGEN_CHUNK_ID) == 0)
				return ResourceType::Song;
			else if (utils::chunkcmp(chunk_id, chunk_reader::TALK_CHUNK_ID) == 0)
				return ResourceType::Talkie;
			else if (utils::chunkcmp(chunk_id, chunk_reader::DIGI_CHUNK_ID) == 0)
				return ResourceType::SFX;
			else if (utils::chunkcmp(chunk_id, chunk_reader::SCRP_CHUNK_ID) == 0)
				return ResourceType::Script;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM00_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM01_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM02_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM03_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM04_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM05_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM06_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM07_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM08_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM09_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM11_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM12_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM13_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM14_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM15_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM16_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM17_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM0A_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM0B_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM0C_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM0D_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM0E_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::IM0F_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::DISK_CHUNK_ID) == 0)
				return ResourceType::Room;
			else if (utils::chunkcmp(chunk_id, chunk_reader::RNAM_CHUNK_ID) == 0)
				return ResourceType::Room;

			return ResourceType::Unknown;
		}
	}
}