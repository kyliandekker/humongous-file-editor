#pragma once

#include <string>

namespace HumongousFileEditor
{
	namespace files
	{
		public enum ResourceType
		{
			Unknown,
			Talkie,
			SFX,
			Song,
			RoomBackground,
			RoomPicture,
			Script,
			Room
		};

		ResourceType GetResourceTypeByChunkName(unsigned char* chunk_id);
	}
}