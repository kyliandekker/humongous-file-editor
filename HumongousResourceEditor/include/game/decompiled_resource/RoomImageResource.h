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
			static bool GetRoomImageData(game::GameResource& a_Resource, ImgInfo& a_ImageInfo);
			bool GetData(game::GameResource& a_Resource);
			bool ReplaceResource(game::GameResource& a_Resource) override;
		};
	}
}