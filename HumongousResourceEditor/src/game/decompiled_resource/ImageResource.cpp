#include "game/decompiled_resource/ImageResource.h"

#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "system/audio/AudioUtils.h"
#include "imgui/ImGuiWindow.h"

#include <vector>

namespace resource_editor
{
	namespace game
	{
		ImageResource::ImageResource(game::GameResource& a_Resource)
		{
			GetData(a_Resource);
		}

		ImageResource::ImageResource(const ImageResource& rhs)
		{ }

		ImageResource::~ImageResource()
		{
			if (m_Texture)
			{
				m_Texture->Release();
				m_Texture = nullptr;
			}
		}

		bool ImageResource::GetData(game::GameResource& a_Resource)
		{
			return true;
		}

		std::vector<uint8_t> CreateBitstream(unsigned char* a_Data, size_t a_Length)
		{
			std::vector<uint8_t> bits;
			for (size_t i = 0; i < a_Length; ++i)
			{
				char c = a_Data[i];
				for (int j = 0; j < 8; j++)
				{
					bits.push_back((c >> j) & 1);
				}
			}
			return bits;
		}

		uint8_t CollectBits(int& a_Pos, std::vector<uint8_t>& a_Bitstream, int a_Count)
		{
			int result = 0;
			for (int i = 0; i < a_Count; i++)
			{
				result |= a_Bitstream[a_Pos++] << i;
			}

			return result;
		}

		bool ImageResource::DecodeHE(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info)
		{
			std::vector<uint8_t> out;

			unsigned char color = a_FillColor;

			size_t num_pixels = a_Info.m_Width * a_Info.m_Height;

			if (a_DataSize == 0)
			{
				for (size_t i = 0; i < num_pixels; i++)
				{
					out.push_back(color % 256);
				}
			}
			else
			{
				std::vector<int> delta_color = { -4, -3, -2, -1, 1, 2, 3, 4 };

				std::vector<uint8_t> bits = CreateBitstream(a_Data, a_DataSize);

				out.push_back(color % 256);

				int pos = 0;
				while (out.size() < num_pixels)
				{
					if (bits[pos++] == 1)
					{
						if (bits[pos++] == 1)
						{
							uint8_t bitc = CollectBits(pos, bits, 3);
							color += delta_color[bitc];
						}
						else
						{
							color = CollectBits(pos, bits, a_Palen);
						}
					}
					out.push_back(color % 256);
				};
			}

			a_Info.m_Size = out.size();
			a_Info.m_Data = reinterpret_cast<unsigned char*>(malloc(a_Info.m_Size));
			if (!a_Info.m_Data)
			{
				return false;
			}
			memcpy(a_Info.m_Data, out.data(), a_Info.m_Size);
			return true;
		}

		bool ImageResource::DecodeBasic(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, size_t a_Width, size_t a_Height, int a_Palen, bool a_Transparent, ImgInfo& a_Info)
		{
			unsigned char color = a_FillColor;

			std::vector<uint8_t> bits = CreateBitstream(a_Data, a_DataSize);

			std::vector<uint8_t> out;

			a_Info.m_Channels = 3;
			if (a_Transparent)
				a_Info.m_Channels = 4;

			out.push_back(color % 256);

			size_t num_pixels = a_Info.m_Width * a_Info.m_Height;

			int sub = 1;
			int pos = 0;
			while (out.size() < num_pixels)
			{
				if (bits[pos++] == 1)
				{
					if (bits[pos++] == 1)
					{
						if (bits[pos++] == 1)
							sub = -sub;
						color -= sub;
					}
					else
					{
						color = CollectBits(pos, bits, a_Palen);
						sub = 1;
					}
				}
				out.push_back(color % 256);
			};

			a_Info.m_Size = out.size();
			a_Info.m_Data = reinterpret_cast<unsigned char*>(malloc(a_Info.m_Size));
			if (!a_Info.m_Data)
			{
				return false;
			}
			memcpy(a_Info.m_Data, out.data(), a_Info.m_Size);
			return true;
		}

		bool ImageResource::DecodeMajmin(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info)
		{
			unsigned char color = a_FillColor;

			std::vector<uint8_t> bits = CreateBitstream(a_Data, a_DataSize);
			std::vector<uint8_t> out;

			a_Info.m_Channels = 3;
			if (a_Transparent)
				a_Info.m_Channels = 4;

			out.push_back(color % 256);

			size_t num_pixels = a_Info.m_Width * a_Info.m_Height;

			int pos = 0;
			while (out.size() < num_pixels)
			{
				if (bits[pos++] == 1)
				{
					if (bits[pos++] == 1)
					{
						uint8_t shift = CollectBits(pos, bits, 3) - 4;
						if (shift != 0)
						{
							color += shift;
						}
						else
						{
							uint8_t ln = CollectBits(pos, bits, 8) - 1;
							for (size_t i = 0; i < ln; i++)
								out.push_back((color % 256));
						}
					}
					else
					{
						color = CollectBits(pos, bits, a_Palen);
					}
				}
				out.push_back(color % 256);
			};

			a_Info.m_Size = out.size();
			a_Info.m_Data = reinterpret_cast<unsigned char*>(malloc(a_Info.m_Size));
			if (!a_Info.m_Data)
			{
				return false;
			}
			memcpy(a_Info.m_Data, out.data(), a_Info.m_Size);
			return true;
		}

		bool ImageResource::DecodeRaw(unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info)
		{
			a_Info.m_Size = a_Info.m_Width * a_Info.m_Height;
			a_Info.m_Data = reinterpret_cast<unsigned char*>(malloc(a_Info.m_Size));
			if (!a_Info.m_Data)
			{
				return false;
			}
			memcpy(a_Info.m_Data, a_Data, a_Info.m_Size);
			return true;
		}

		bool ImageResource::GetDataBMAP(chunk_reader::BMAP_Chunk& a_BMAP_Chunk, chunk_reader::APAL_Chunk& a_APAL_Chunk, uint8_t a_FillColor, size_t a_Width, size_t a_Height)
		{
			if (low_level::utils::chunkcmp(a_BMAP_Chunk.chunk_id, chunk_reader::BMAP_CHUNK_ID) != 0)
			{
				return false;
			}

			if (low_level::utils::chunkcmp(a_APAL_Chunk.chunk_id, chunk_reader::APAL_CHUNK_ID) != 0)
			{
				return false;
			}

			size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(a_BMAP_Chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
			size_t bmap_size = a_BMAP_Chunk.ChunkSize() - header_size;

			int palen = a_BMAP_Chunk.encoding % 10;

			bool he = a_BMAP_Chunk.encoding >= 0x86 && a_BMAP_Chunk.encoding <= 0x8A;
			bool he_transparent = a_BMAP_Chunk.encoding >= 0x90 && a_BMAP_Chunk.encoding <= 0x94;

			m_ImageInfo.m_Width = a_Width;
			m_ImageInfo.m_Height = a_Height;

			if (!DecodeHE(a_FillColor, a_BMAP_Chunk.data, bmap_size, palen, he_transparent, m_ImageInfo))
			{
				return false;
			}

			m_ImageInfo.m_Channels = 4;

			std::vector<uint8_t> newOut;
			for (size_t i = 0; i < m_ImageInfo.m_Size; i++)
			{
				newOut.push_back(a_APAL_Chunk.data[m_ImageInfo.m_Data[i] * 3]);
				newOut.push_back(a_APAL_Chunk.data[m_ImageInfo.m_Data[i] * 3 + 1]);
				newOut.push_back(a_APAL_Chunk.data[m_ImageInfo.m_Data[i] * 3 + 2]);
				if (m_ImageInfo.m_Data[i] == a_FillColor)
				{
					newOut.push_back(0);
				}
				else
				{
					newOut.push_back(255);
				}
			}

			m_ImageInfo.m_Size = newOut.size();
			if (m_ImageInfo.m_Data)
			{
				free(m_ImageInfo.m_Data);
			}
			m_ImageInfo.m_Data = reinterpret_cast<unsigned char*>(malloc(m_ImageInfo.m_Size));
			if (!m_ImageInfo.m_Data)
			{
				return false;
			}
			memcpy(m_ImageInfo.m_Data, newOut.data(), m_ImageInfo.m_Size);

			imgui::window.GetDX9().CreateTexture(m_Texture, m_ImageInfo);

			return true;
		}
	}
}