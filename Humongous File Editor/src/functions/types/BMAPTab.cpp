#include "functions/types/BMAPTab.h"

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "functions/TabFunctions.h"
#include "functions/ChunkFunctions.h"

namespace HumongousFileEditor
{
	struct offset_pair
	{
		size_t start, end;
	};

	struct strip
	{
		unsigned char* data;
		size_t size;
	};

	bool BMAPTab::GetDataBMAP(chunk_reader::FileContainer*& fc, chunk_reader::BMAP_Chunk& bmap_chunk, chunk_reader::APAL_Chunk& apal_chunk, uint8_t fill_color, size_t width, size_t height, img_info& info)
	{
		size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
		size_t bmap_size = bmap_chunk.ChunkSize() - header_size;

		int palen = bmap_chunk.encoding % 10;

		bool he = bmap_chunk.encoding >= 0x86 && bmap_chunk.encoding <= 0x8A;
		bool he_transparent = bmap_chunk.encoding >= 0x90 && bmap_chunk.encoding <= 0x94;

		if (!DecodeHE(fill_color, bmap_chunk.data, bmap_size, width, height, palen, he_transparent, info))
			return false;

		std::vector<uint8_t> newOut;
		for (size_t i = 0; i < info.size; i++)
		{
			newOut.push_back(apal_chunk.data[info.data[i] * 3]);
			newOut.push_back(apal_chunk.data[info.data[i] * 3 + 1]);
			newOut.push_back(apal_chunk.data[info.data[i] * 3 + 2]);
			if (he_transparent)
			{
				if (info.data[i] == fill_color)
					newOut.push_back(0);
				else
					newOut.push_back(255);
			}
		}

		free(info.data);

		info.size = newOut.size();
		info.data = reinterpret_cast<unsigned char*>(malloc(newOut.size()));
		memcpy(info.data, newOut.data(), newOut.size());

		return true;
	}

	bool BMAPTab::GetDataSMAP(chunk_reader::FileContainer*& fc, chunk_reader::OBIM_Chunk& obim_chunk, size_t width, size_t height, chunk_reader::SMAP_Chunk& smap_chunk, chunk_reader::APAL_Chunk& apal_chunk, img_info& info)
	{
		size_t header_size = sizeof(chunk_reader::SMAP_Chunk) - sizeof(smap_chunk.data); // Pointer in the SMAP class is size 8 and needs to be deducted.
		size_t smap_size = smap_chunk.ChunkSize() - header_size;

		uint32_t strip_width = 8;
		size_t num_strips = static_cast<size_t>(floor(width / strip_width));

		unsigned char* data = smap_chunk.data;

		std::vector<uint32_t> offsets;
		int j = 0;
		for (size_t i = 0; i < num_strips; i++, j += sizeof(uint32_t))
		{
			uint32_t number = *reinterpret_cast<uint32_t*>(utils::add(smap_chunk.data, j));
			number -= sizeof(chunk_reader::HumongousHeader);
			offsets.push_back(number);
		}

		std::vector<offset_pair> index;

		for (size_t i = 0; i < offsets.size(); i++)
			index.push_back({ offsets[i], (i + 1) == offsets.size() ? smap_size : offsets[i + 1] });

		std::vector<strip> strips;
		for (size_t i = 0; i < num_strips; i++)
			strips.push_back({ utils::add(smap_chunk.data, index[i].start), index[i].end - index[i].start });

		size_t total_size = 0;
		std::vector< std::vector<std::vector<uint8_t>>> data_blocks;
		for (size_t i = 0; i < strips.size(); i++)
		{
			strip& strip = strips[i];

			std::vector<std::vector<uint8_t>> data_new_block;

			uint8_t code = strip.data[0];

			bool horizontal = true;
			if (code >= 0x03 && code <= 0x12 || code >= 0x22 && code <= 0x26)
				horizontal = false;

			bool he_transparent = code >= 0x22 && code <= 0x30 || code >= 0x54 && code <= 0x80 || code >= 0x8F;


			uint8_t transparency = 0;
			if (he_transparent)
				transparency = 255;

			int palen = code % 10;

			uint8_t color = strip.data[1];

			img_info strip_info;
			if (code >= 0x40 && code <= 0x80)
			{
				if (!DecodeMajmin(color, utils::add(strip.data, 2), strip.size - 2, strip_width, height, palen, he_transparent, strip_info))
					return false;
			}
			else if (code >= 0x0E && code <= 0x30)
			{
				if (!DecodeBasic(color, utils::add(strip.data, 2), strip.size - 2, strip_width, height, palen, he_transparent, strip_info))
					return false;
			}
			else if (code >= 0x86 && code <= 0x94)
			{
				if (!DecodeHE(color, utils::add(strip.data, 2), strip.size - 2, strip_width, height, palen, he_transparent, strip_info))
					return false;
			}
			else if (code >= 0x01 && code <= 0x95)
			{
				if (!DecodeRaw(strip.data, strip.size, strip_width, height, palen, he_transparent, strip_info))
					return false;
			}

			unsigned char* data = reinterpret_cast<unsigned char*>(malloc(strip_info.size));

			if (horizontal)
				memcpy(data, strip_info.data, strip_info.size);
			else
			{
				int dataIndex = 0;
				for (int k = 0; k < strip_width; ++k)
				{
					for (int j = 0; j < height; ++j)
					{
						data[(j * strip_width) + k] = strip_info.data[dataIndex];
						++dataIndex;
					}
				}
			}

			free(strip_info.data);
			strip_info.data = data;
			total_size += strip_info.size;

			for (size_t k = 0; k < height; k++)
			{
				std::vector<uint8_t> new_strip;
				for (size_t j = 0; j < strip_width; j++)
				{
					new_strip.push_back(strip_info.data[(k * strip_width) + j]);
				}
				data_new_block.push_back(new_strip);
			}

			data_blocks.push_back(data_new_block);
		}

		std::vector<std::vector<uint8_t>> finals;

		for (size_t i = 0; i < data_blocks[0].size(); i++)
		{
			std::vector<uint8_t> arr;
			for (size_t j = 0; j < data_blocks.size(); j++)
			{
				for (size_t h = 0; h < strip_width; h++)
				{
					arr.push_back(data_blocks[j][i][h]);
				}
			}
			finals.push_back(arr);
		}

		unsigned char* total_data = reinterpret_cast<unsigned char*>(malloc(total_size));
		int pos = 0;
		for (size_t i = 0; i < finals.size(); i++)
		{
			std::vector<uint8_t>& arr = finals[i];
			for (size_t j = 0; j < arr.size(); j++)
			{
				memcpy(reinterpret_cast<unsigned char*>(utils::add(total_data, pos)), &finals[i][j], sizeof(uint8_t));
				pos++;
			}
		}

		info.width = width;
		info.height = height;
		info.data = total_data;
		info.size = total_size;
		info.channels = 4;

		std::vector<uint8_t> newOut;
		for (size_t i = 0; i < info.size; i++)
		{
			newOut.push_back(apal_chunk.data[info.data[i] * 3]);
			newOut.push_back(apal_chunk.data[info.data[i] * 3 + 1]);
			newOut.push_back(apal_chunk.data[info.data[i] * 3 + 2]);
			newOut.push_back(255);
		}

		free(info.data);

		info.size = newOut.size();
		info.data = reinterpret_cast<unsigned char*>(malloc(newOut.size()));
		memcpy(info.data, newOut.data(), newOut.size());

		return true;
	}
}