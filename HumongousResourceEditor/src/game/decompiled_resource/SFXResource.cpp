#include "game/decompiled_resource/SFXResource.h"

#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "system/audio/AudioUtils.h"

#include <vector>

namespace resource_editor
{
	namespace game
	{
		SFXResource::SFXResource(game::GameResource& a_Resource) : SoundResource(a_Resource)
		{
			GetData(a_Resource);
		}

		SFXResource::SFXResource(const SFXResource& rhs) : SoundResource(rhs)
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

		bool SFXResource::GetData(game::GameResource& a_Resource)
		{
			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);
			if (children.size() == 0)
			{
				return false;
			}

			std::vector<chunk_reader::ChunkInfo> desired = {
				chunk_reader::ChunkInfo(chunk_reader::HSHD_CHUNK_ID),
				chunk_reader::ChunkInfo(chunk_reader::SDAT_CHUNK_ID),
			};

			if (low_level::utils::seekChildren(children, desired) < desired.size())
			{
				return false;
			}

			a_Resource.m_Parent->m_FileContainer.GetChunk(m_HSHD_Chunk, desired[0].m_Offset);

			a_Resource.m_Parent->m_FileContainer.GetChunk(m_SDAT_Chunk, desired[1].m_Offset, sizeof(chunk_reader::HumongousHeader));
			m_SDAT_Chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.data(), desired[1].m_Offset + sizeof(chunk_reader::HumongousHeader));

			m_NumSamples = m_SDAT_Chunk.ChunkSize();
			m_Samples = audio::utils::ToSample(m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize());

			a_Resource.m_Properties.emplace(std::make_pair("4. Sample Rate", std::string(std::to_string(m_HSHD_Chunk.sample_rate) + "Hz")));
			a_Resource.m_Properties.emplace(std::make_pair("5. Channels", std::to_string(1) + " (mono)"));
			a_Resource.m_Properties.emplace(std::make_pair("6. Bits Per Sample", std::to_string(8)));
			a_Resource.m_Properties.emplace(std::make_pair("7. Size", std::string(std::to_string(m_SDAT_Chunk.ChunkSize()) + " bytes")));

			return true;
		}

        bool SFXResource::Replace(game::GameResource& a_Resource)
        {
            return false;
        }

		bool SFXResource::Save(game::GameResource& a_Resource)
		{
			std::string path;
			if (SaveResource(path))
			{
				const bool saved = SaveSound(path, m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize() - sizeof(resource_editor::chunk_reader::SDAT_Chunk), m_HSHD_Chunk.sample_rate);
				if (saved)
				{
					system(path.c_str());
				}
				return saved;
			}
			return false;
		}
	}
}