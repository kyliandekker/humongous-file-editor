#pragma once

#include <windows.h>
#include <d3d9.h>

namespace resource_editor
{
	namespace game
	{
		class ImgInfo;
	}

	namespace dx9
	{
		class DX9Window
		{
		public:
			DX9Window();
			~DX9Window();

			void Init(HWND hwnd);
			void SetRenderState();
			void Clear();
			bool BeginScene();
			bool EndScene();
			bool CreateDeviceD3D(HWND hWnd);
			void CleanupDeviceD3D();
			void ResetDevice();
			bool CreateTexture(PDIRECT3DTEXTURE9* out_texture, game::ImgInfo& a_Info);

			_D3DPRESENT_PARAMETERS_* g_d3dpp = nullptr;
			IDirect3D9* g_pD3D = nullptr;
			IDirect3DDevice9* g_pd3dDevice = nullptr;
		};
	}
}