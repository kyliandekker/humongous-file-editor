#pragma once

#include "./DecompiledResource.h"

#include <vector>
#include <stdint.h>
#include <D3dx9tex.h>
#include <mutex>

#include "./ImgInfo.h"

namespace resource_editor
{
	namespace chunk_reader
	{
		class BMAP_Chunk;
		class APAL_Chunk;
		class SMAP_Chunk;
	}
}

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class ImageResource : public DecompiledResource
		{
		public:
			ImageResource(game::GameResource& a_Resource);
			ImageResource() = default;
			ImageResource(const ImageResource& rhs);
			~ImageResource() override;
			static bool GetData(game::GameResource& a_Resource);
			static bool DecodeHE(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info);
			static bool DecodeBasic(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info);
			static bool DecodeMajmin(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info);
			static bool DecodeRaw(unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info);
			static bool GetDataBMAP(chunk_reader::BMAP_Chunk& a_BMAP_Chunk, chunk_reader::APAL_Chunk& a_APAL_Chunk, uint8_t a_FillColor, size_t a_Width, size_t a_Height, ImgInfo& a_ImageInfo, bool a_ShowTransparency = false);
			static bool GetDataSMAP(chunk_reader::SMAP_Chunk& a_SMAP_Chunk, chunk_reader::APAL_Chunk& a_APAL_Chunk, size_t a_Width, size_t a_Height, ImgInfo& a_ImageInfo, bool a_ShowTransparency = false);
			ImgInfo m_ImageInfo;
			bool m_ShowTransparency = false;
			PDIRECT3DTEXTURE9 m_Texture = nullptr;
			std::mutex m_ImageMutex;
		};
	}
}