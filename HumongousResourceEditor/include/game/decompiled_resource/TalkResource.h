#pragma once

#include "./SoundResource.h"

#include "low_level/HumongousChunks.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class TalkResource : public SoundResource
		{
		public:
			TalkResource() = default;
			TalkResource(game::GameResource& a_Resource);
			TalkResource(const TalkResource& rhs);
			~TalkResource();
			bool GetData(game::GameResource& a_Resource);
			bool Replace(game::GameResource& a_Resource) override;
			bool Save(game::GameResource& a_Resource) override;
		};
	}
}