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
			static bool GetRoomImageData(game::GameResource& a_Resource, ImgInfo& a_ImageInfo, bool a_ShowTransparency = false);
			bool GetData(game::GameResource& a_Resource);
			bool Replace(game::GameResource& a_Resource) override;
			bool Save(game::GameResource& a_Resource) override;
		};
	}
}