#pragma once

namespace resource_editor
{
	namespace audio
	{
		namespace utils
		{
			double* ToSample(unsigned char* data, size_t buffersize, size_t numSamples);
		}
	}
}