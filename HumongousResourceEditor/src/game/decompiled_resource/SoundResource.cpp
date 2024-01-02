#include "game/decompiled_resource/SoundResource.h"

#include "game/GameResource.h"
#include "project/Resource.h"

#include <vector>

namespace resource_editor
{
	namespace game
	{
		SoundResource::SoundResource(game::GameResource& a_Resource)
		{
			GetData(a_Resource);
		}

		SoundResource::SoundResource(const SoundResource& rhs)
		{
			m_HSHD_Chunk = rhs.m_HSHD_Chunk;
			m_SDAT_Chunk = rhs.m_SDAT_Chunk;
			m_NumSamples = rhs.m_NumSamples;
			m_Samples = reinterpret_cast<double*>(malloc(m_NumSamples * sizeof(double)));
			if (m_Samples)
			{
				memcpy(m_Samples, rhs.m_Samples, m_NumSamples * sizeof(double));
			}
		}

		SoundResource::~SoundResource()
		{
			if (m_Samples)
			{
				free(m_Samples);
			}
			if (m_SDAT_Chunk.data)
			{
				free(m_SDAT_Chunk.data);
			}
		}

		bool SoundResource::GetData(game::GameResource& a_Resource)
		{
			return true;
		}
	}
}