#pragma once

#include <xaudio2.h>

class VoiceCallback : public IXAudio2VoiceCallback
{
public:
	void STDMETHODCALLTYPE OnStreamEnd() override
	{}
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override
	{}
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 SamplesRequired) override
	{}
	void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override
	{}
	void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) override
	{}
	void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext) override
	{}
	void STDMETHODCALLTYPE OnVoiceError(void* pBufferContext, HRESULT Error) override
	{}
};