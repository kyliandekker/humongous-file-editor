#pragma once

#include "./ImageResource.h"

#include "low_level/HumongousChunks.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class RoomBackgroundResource : public ImageResource
		{
		public:
			RoomBackgroundResource(game::GameResource& a_Resource);
			RoomBackgroundResource() = default;
			RoomBackgroundResource(const RoomBackgroundResource& rhs);
			~RoomBackgroundResource();
			bool GetData(game::GameResource& a_Resource);
		};
	}
}