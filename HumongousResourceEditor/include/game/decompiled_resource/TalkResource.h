#pragma once

#include "./DecompiledResource.h"

#include "low_level/HumongousChunks.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class TalkResource : public DecompiledResource
		{
		public:
			TalkResource(game::GameResource& a_Resource);
			bool GetData(game::GameResource& a_Resource);
			chunk_reader::HSHD_Chunk m_HSHD_Chunk;
			chunk_reader::SDAT_Chunk m_SDAT_Chunk;
			size_t m_NumSamples = 0;
			double* m_Samples = nullptr;
		};
	}
}