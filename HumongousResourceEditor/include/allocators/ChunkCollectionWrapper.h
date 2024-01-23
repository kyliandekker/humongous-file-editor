#pragma once

#include <uaudio_wave_reader/ChunkCollection.h>

namespace uaudio
{
	namespace wave_reader
	{
		class ChunkCollectionWrapper : public ChunkCollection
		{
		public:
			ChunkCollectionWrapper() = default;
			ChunkCollectionWrapper(size_t a_Size);
			ChunkCollectionWrapper(void* a_Ptr, size_t a_Size);
			~ChunkCollectionWrapper();

			ChunkCollectionWrapper(const ChunkCollectionWrapper& rhs);
			ChunkCollectionWrapper& operator=(const ChunkCollectionWrapper& rhs);
		};
	}
}