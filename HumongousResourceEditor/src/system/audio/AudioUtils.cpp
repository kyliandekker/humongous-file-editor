#include "system/audio/AudioUtils.h"

#include <cstdint>
#include <corecrt_malloc.h>

namespace resource_editor
{
	namespace audio
	{
		namespace utils
		{
			double* ToSample(unsigned char* data, size_t buffersize, size_t numSamples)
			{
				if (data == nullptr)
				{
					return nullptr;
				}

				unsigned char* pData = data;
				size_t realNumSamples = buffersize;

				if (realNumSamples == 0)
				{
					return nullptr;
				}
				size_t div = realNumSamples / numSamples;

				double* samples = reinterpret_cast<double*>(malloc(numSamples * sizeof(double)));
				if (!samples)
				{
					return nullptr;
				}

				for (size_t i = 0; i < numSamples; i++)
				{
					int8_t psample = INT8_MAX - *(uint8_t*)pData;
					samples[i] = static_cast<double>(psample) / INT8_MAX;

					pData += div;
				}

				return samples;
			}
		}
	}
}