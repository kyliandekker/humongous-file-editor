#pragma once

#include "./ImageResource.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class RoomImageLayerResource : public ImageResource
		{
		public:
			RoomImageLayerResource(game::GameResource& a_Resource);
			RoomImageLayerResource() = default;
			RoomImageLayerResource(const RoomImageLayerResource& rhs);
			~RoomImageLayerResource();
			bool GetData(game::GameResource& a_Resource);
			bool ReplaceResource(game::GameResource& a_Resource) override;
		};
	}
}