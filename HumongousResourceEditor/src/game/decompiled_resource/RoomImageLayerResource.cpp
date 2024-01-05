#include "game/decompiled_resource/RoomImageLayerResource.h"

#include "low_level/HumongousChunks.h"

#include <vector>

namespace resource_editor
{
	namespace game
	{
		RoomImageLayerResource::RoomImageLayerResource(game::GameResource& a_Resource) : ImageResource(a_Resource)
		{
			GetData(a_Resource);
		}

		RoomImageLayerResource::RoomImageLayerResource(const RoomImageLayerResource& rhs) : ImageResource(rhs)
		{
		}

		RoomImageLayerResource::~RoomImageLayerResource()
		{
		}

		bool RoomImageLayerResource::GetData(game::GameResource& a_Resource)
		{
			return false;
		}
	}
}