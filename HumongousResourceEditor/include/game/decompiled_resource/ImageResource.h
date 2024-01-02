#pragma once

#include "./DecompiledResource.h"

#include <vector>
#include <stdint.h>
#include <D3dx9tex.h>

#include "low_level/HumongousChunks.h"
#include "./ImgInfo.h"

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
			~ImageResource();
			bool GetData(game::GameResource& a_Resource);
			bool DecodeHE(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info);
			bool DecodeBasic(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, size_t a_Width, size_t a_Height, int a_Palen, bool a_Transparent, ImgInfo& a_Info);
			bool DecodeMajmin(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info);
			bool DecodeRaw(unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info);
			bool GetDataBMAP(chunk_reader::BMAP_Chunk& a_BMAP_Chunk, chunk_reader::APAL_Chunk& a_APAL_Chunk, uint8_t a_FillColor, size_t a_Width, size_t a_Height);
			ImgInfo m_ImageInfo;
			PDIRECT3DTEXTURE9 m_Texture;
		};
	}
}