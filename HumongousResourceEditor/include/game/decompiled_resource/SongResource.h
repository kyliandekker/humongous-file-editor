#pragma once

#include "./SoundResource.h"

#include "low_level/HumongousChunks.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class SongResource : public SoundResource
		{
		public:
			SongResource(game::GameResource& a_Resource);
			SongResource() = default;
			SongResource(const SongResource& rhs);
			~SongResource();
			bool GetData(game::GameResource& a_Resource);
			bool ReplaceResource(game::GameResource& a_Resource) override;
		};
	}
}