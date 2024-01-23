#pragma once

#include "./DecompiledResource.h"

#include <string>

#include "low_level/HumongousChunks.h"

namespace uaudio
{
	namespace wave_reader
	{
		class ChunkCollectionWrapper;
	}
}

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class SoundResource : public DecompiledResource
		{
		public:
			SoundResource(game::GameResource& a_Resource);
			SoundResource() = default;
			SoundResource(const SoundResource& rhs);
			~SoundResource() override;
			bool GetData(game::GameResource& a_Resource) override;
			bool OpenResource(std::string& a_Path, uaudio::wave_reader::ChunkCollectionWrapper& a_ChunkCollection);
			bool SaveResource(std::string& a_Path);
			bool SaveSound(std::string a_Path, void* a_Data, size_t a_DataSize, uint16_t a_SampleRate);
			chunk_reader::HSHD_Chunk m_HSHD_Chunk;
			chunk_reader::SDAT_Chunk m_SDAT_Chunk;
			size_t m_NumSamples = 0;
			double* m_Samples = nullptr;
		};
	}
}