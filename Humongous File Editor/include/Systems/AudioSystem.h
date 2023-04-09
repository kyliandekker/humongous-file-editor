#pragma once

#include <cstdint>
#include "VoiceCallback.h"

class IXAudio2;
class IXAudio2MasteringVoice;
class IXAudio2SourceVoice;

namespace hfe
{
	class AudioSystem
	{
	public:
		AudioSystem();
		~AudioSystem();

		void Play(unsigned char* data, uint32_t size);
		void Stop();

	private:
		IXAudio2* m_Engine = nullptr;
		IXAudio2MasteringVoice* m_MasterVoice = nullptr;
		IXAudio2SourceVoice* m_SourceVoice = nullptr;
		VoiceCallback vcb;
	};
	extern AudioSystem audioSystem;
}