#pragma once

#include "./SoundResource.h"

#include "low_level/HumongousChunks.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class SFXResource : public SoundResource
		{
		public:
			SFXResource(game::GameResource& a_Resource);
			SFXResource() = default;
			SFXResource(const SFXResource& rhs);
			~SFXResource();
			bool GetData(game::GameResource& a_Resource);
			bool Replace(game::GameResource& a_Resource) override;
			bool Save(game::GameResource& a_Resource) override;
		};
	}
}