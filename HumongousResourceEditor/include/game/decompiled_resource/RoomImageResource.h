#pragma once

#include "./ImageResource.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class RoomImageResource : public ImageResource
		{
		public:
			RoomImageResource(game::GameResource& a_Resource);
			RoomImageResource() = default;
			RoomImageResource(const RoomImageResource& rhs);
			~RoomImageResource();
			bool GetData(game::GameResource& a_Resource);
		};
	}
}