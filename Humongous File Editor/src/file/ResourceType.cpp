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
			else if (utils::chunkcmp(chunk_id, chunk_reader::RMIM_CHUNK_ID) == 0)
				return ResourceType::RoomBackground;
			else if (utils::chunkcmp(chunk_id, chunk_reader::OBIM_CHUNK_ID) == 0)
				return ResourceType::RoomImage;
			else if (utils::chunkcmp(chunk_id, chunk_reader::DISK_CHUNK_ID) == 0)
				return ResourceType::Room;
			else if (utils::chunkcmp(chunk_id, chunk_reader::RNAM_CHUNK_ID) == 0)
				return ResourceType::Room;

			return ResourceType::Unknown;
		}
	}
}