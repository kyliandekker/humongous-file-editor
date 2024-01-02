#include "system/audio/AudioUtils.h"

#include <cstdint>
#include <corecrt_malloc.h>

namespace resource_editor
{
	namespace audio
	{
		namespace utils
		{
			double* ToSample(unsigned char* a_Data, size_t a_Buffersize)
			{
				if (a_Data == nullptr)
				{
					return nullptr;
				}

				unsigned char* pData = a_Data;

				if (a_Buffersize == 0)
				{
					return nullptr;
				}

				double* samples = reinterpret_cast<double*>(malloc(a_Buffersize * sizeof(double)));
				if (!samples)
				{
					return nullptr;
				}

				for (size_t i = 0; i < a_Buffersize; i++)
				{
					int8_t psample = INT8_MAX - *(uint8_t*)pData;
					samples[i] = static_cast<double>(psample) / INT8_MAX;

					pData += 1;
				}

				return samples;
			}
		}
	}
}