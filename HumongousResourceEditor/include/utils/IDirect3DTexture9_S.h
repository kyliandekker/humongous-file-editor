#pragma once

#include <D3dx9tex.h>
#include <mutex>

namespace resource_editor
{
	struct IDirect3DTexture9_S : public IDirect3DTexture9
	{
		std::mutex m_Mutex;
	};
	typedef struct IDirect3DTexture9_S* PDIRECT3DTEXTURE9_S;
}