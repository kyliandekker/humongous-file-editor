#include "game/decompiled_resource/ImageResource.h"

#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "system/audio/AudioUtils.h"
#include "imgui/ImGuiWindow.h"
#include "low_level/HumongousChunks.h"

#include <vector>

namespace resource_editor
{
	namespace game
	{
		ImageResource::ImageResource(game::GameResource& a_Resource) : DecompiledResource()
		{
			GetData(a_Resource);
		}

		ImageResource::ImageResource(const ImageResource& rhs) : DecompiledResource(rhs)
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

			a_Info.m_Data = DataContainer(out.size(), out.data());
			return true;
		}

		bool ImageResource::DecodeBasic(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info)
		{
			unsigned char color = a_FillColor;

			std::vector<uint8_t> bits = CreateBitstream(a_Data, a_DataSize);

			std::vector<uint8_t> out;

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

			a_Info.m_Data = DataContainer(out.size(), out.data());
			return true;
		}

		bool ImageResource::DecodeMajmin(unsigned char a_FillColor, unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info)
		{
			unsigned char color = a_FillColor;

			std::vector<uint8_t> bits = CreateBitstream(a_Data, a_DataSize);
			std::vector<uint8_t> out;

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
							{
								out.push_back((color % 256));
							}
						}
					}
					else
					{
						color = CollectBits(pos, bits, a_Palen);
					}
				}
				out.push_back(color % 256);
			};

			a_Info.m_Data = DataContainer(out.size(), out.data());
			return true;
		}

		bool ImageResource::DecodeRaw(unsigned char* a_Data, size_t a_DataSize, int a_Palen, bool a_Transparent, ImgInfo& a_Info)
		{
			a_Info.m_Data = ImageData(a_Info.m_Width * a_Info.m_Height, a_Data);
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

			if (a_Width == 0 || a_Height == 0)
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

			std::vector<uint8_t> newOut;
			for (size_t i = 0; i < m_ImageInfo.Size(); i++)
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

			m_ImageInfo.m_Data = DataContainer(newOut.size(), newOut.data());

			imgui::window.GetDX9().CreateTexture(m_Texture, m_ImageInfo);

			return true;
		}

		struct pair
		{
			size_t rmim_offset;
			size_t offset;
			size_t actual_offset;
		};

		struct offset_pair
		{
			size_t start, end;
		};

		struct strip
		{
			unsigned char* data;
			size_t size;
		};

		struct IndexColor
		{
			uint8_t index = 0;
			uint8_t trans_color = 0;

			IndexColor(uint8_t index, uint8_t trans_color)
			{
				this->index = index;
				this->trans_color = trans_color;
			}

			IndexColor()
			{}
		};

		bool ImageResource::GetDataSMAP(chunk_reader::SMAP_Chunk& a_SMAP_Chunk, chunk_reader::APAL_Chunk& a_APAL_Chunk, size_t a_Width, size_t a_Height)
		{
			if (low_level::utils::chunkcmp(a_SMAP_Chunk.chunk_id, chunk_reader::SMAP_CHUNK_ID) != 0)
			{
				return false;
			}

			if (low_level::utils::chunkcmp(a_APAL_Chunk.chunk_id, chunk_reader::APAL_CHUNK_ID) != 0)
			{
				return false;
			}

			if (a_Width == 0 || a_Height == 0)
			{
				return false;
			}

			size_t header_size = sizeof(chunk_reader::SMAP_Chunk) - sizeof(a_SMAP_Chunk.data); // Pointer in the SMAP class is size 8 and needs to be deducted.
			size_t smap_size = a_SMAP_Chunk.ChunkSize() - header_size;

			uint32_t strip_width = 8;
			size_t num_strips = static_cast<size_t>(static_cast<size_t>(floor(static_cast<double>(a_Width / strip_width))));

			std::vector<uint32_t> offsets;
			int j = 0;
			for (size_t i = 0; i < num_strips; i++, j += sizeof(uint32_t))
			{
				uint32_t number = *reinterpret_cast<uint32_t*>(low_level::utils::add(a_SMAP_Chunk.data, j));
				number -= sizeof(chunk_reader::HumongousHeader);
				offsets.push_back(number);
			}

			std::vector<offset_pair> index;

			for (size_t i = 0; i < offsets.size(); i++)
			{
				index.push_back({ offsets[i], (i + 1) == offsets.size() ? smap_size : offsets[i + 1] });
			}

			std::vector<strip> strips;
			for (size_t i = 0; i < num_strips; i++)
			{
				strips.push_back({ low_level::utils::add(a_SMAP_Chunk.data, index[i].start), index[i].end - index[i].start });
			}

			size_t total_size = 0;
			std::vector< std::vector<std::vector<IndexColor>>> data_blocks;

			for (size_t i = 0; i < strips.size(); i++)
			{
				strip& strip = strips[i];

				std::vector<std::vector<IndexColor>> data_new_block;

				uint8_t code = strip.data[0];

				bool horizontal = true;
				if (code >= 0x03 && code <= 0x12 || code >= 0x22 && code <= 0x26)
				{
					horizontal = false;
				}

				bool he_transparent = code >= 0x22 && code <= 0x30 || code >= 0x54 && code <= 0x80 || code >= 0x8F;

				int palen = code % 10;

				uint8_t color = strip.data[1];

				ImgInfo strip_info;
				strip_info.m_Width = strip_width;
				strip_info.m_Height = a_Height;
				if (code >= 0x40 && code <= 0x80)
				{
					if (!DecodeMajmin(color, low_level::utils::add(strip.data, 2), strip.size - 2, palen, he_transparent, strip_info))
					{
						return false;
					}
				}
				else if (code >= 0x0E && code <= 0x30)
				{
					if (!DecodeBasic(color, low_level::utils::add(strip.data, 2), strip.size - 2, palen, he_transparent, strip_info))
					{
						return false;
					}
				}
				else if (code >= 0x86 && code <= 0x94)
				{
					if (!DecodeHE(color, low_level::utils::add(strip.data, 2), strip.size - 2, palen, he_transparent, strip_info))
					{
						return false;
					}
				}
				else if (code >= 0x01 && code <= 0x95)
				{
					if (!DecodeRaw(strip.data, strip.size, palen, he_transparent, strip_info))
					{
						return false;
					}
				}

				DataContainer new_data = DataContainer(strip_info.Size(), strip_info.Data());

				if (!horizontal)
				{
					uint32_t dataIndex = 0;
					for (uint32_t k = 0; k < strip_width; ++k)
					{
						for (uint32_t l = 0; l < static_cast<uint32_t>(a_Height); ++l)
						{
							new_data[(l * strip_width) + k] = strip_info.m_Data[dataIndex];
							++dataIndex;
						}
					}
				}

				strip_info.m_Data = DataContainer(strip_info.Size(), new_data.Data());
				total_size += strip_info.Size();

				for (size_t k = 0; k < a_Height; k++)
				{
					std::vector<IndexColor> new_strip;
					for (size_t b = 0; b < strip_width; b++)
					{
						new_strip.push_back({ strip_info.m_Data[(k * strip_width) + b], color });
					}
					data_new_block.push_back(new_strip);
				}

				data_blocks.push_back(data_new_block);
			}

			std::vector<std::vector<IndexColor>> finals;

			for (size_t i = 0; i < data_blocks[0].size(); i++)
			{
				std::vector<IndexColor> arr;
				for (size_t b = 0; b < data_blocks.size(); b++)
				{
					for (size_t h = 0; h < strip_width; h++)
					{
						arr.push_back(data_blocks[b][i][h]);
					}
				}
				finals.push_back(arr);
			}

			std::vector<IndexColor> final_data = std::vector<IndexColor>(total_size);
			int pos = 0;
			for (size_t i = 0; i < finals.size(); i++)
			{
				std::vector<IndexColor>& arr = finals[i];
				for (size_t b = 0; b < arr.size(); b++)
				{
					final_data[pos] = finals[i][b];
					pos++;
				}
			}

			m_ImageInfo.m_Width = a_Width;
			m_ImageInfo.m_Height = a_Height;

			std::vector<uint8_t> newOut;
			for (size_t i = 0; i < total_size; i++)
			{
				newOut.push_back(a_APAL_Chunk.data[final_data[i].index * 3]);
				newOut.push_back(a_APAL_Chunk.data[final_data[i].index * 3 + 1]);
				newOut.push_back(a_APAL_Chunk.data[final_data[i].index * 3 + 2]);
				if (final_data[i].index == final_data[i].trans_color)
				{
					newOut.push_back(255);
				}
				else
				{
					newOut.push_back(255);
				}
			}

			m_ImageInfo.m_Data = DataContainer(newOut.size(), newOut.data());

			imgui::window.GetDX9().CreateTexture(m_Texture, m_ImageInfo);

			return true;
		}
	}
}