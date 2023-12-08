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

	struct IndexColor
	{
		uint8_t index;
		uint8_t trans_color;

		IndexColor(uint8_t index, uint8_t trans_color)
		{
			this->index = index;
			this->trans_color = trans_color;
		}

		IndexColor()
		{}
	};

	bool BMAPTab::GetDataBMAP(chunk_reader::FileContainer*&, chunk_reader::BMAP_Chunk& bmap_chunk, chunk_reader::APAL_Chunk& apal_chunk, uint8_t fill_color, size_t width, size_t height, img_info& info)
	{
		if (utils::chunkcmp(bmap_chunk.chunk_id, chunk_reader::BMAP_CHUNK_ID) != 0)
			return false;

		if (utils::chunkcmp(apal_chunk.chunk_id, chunk_reader::APAL_CHUNK_ID) != 0)
			return false;

		size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
		size_t bmap_size = bmap_chunk.ChunkSize() - header_size;

		int palen = bmap_chunk.encoding % 10;

		bool he_transparent = bmap_chunk.encoding >= 0x90 && bmap_chunk.encoding <= 0x94;

		if (!DecodeHE(fill_color, bmap_chunk.data, bmap_size, width, height, palen, he_transparent, info))
			return false;

		std::vector<uint8_t> newOut;
		for (size_t i = 0; i < info.size; i++)
		{
			newOut.push_back(apal_chunk.data[info.data.data[i] * 3]);
			newOut.push_back(apal_chunk.data[info.data.data[i] * 3 + 1]);
			newOut.push_back(apal_chunk.data[info.data.data[i] * 3 + 2]);
			if (info.data.data[i] == fill_color)
				newOut.push_back(0);
			else
				newOut.push_back(255);
		}

		info.size = newOut.size();
		info.data = ImageData(newOut.size(), newOut.data());
		info.channels = 4;

		return true;
	}

	bool BMAPTab::GetDataSMAP(chunk_reader::FileContainer*&, chunk_reader::OBIM_Chunk&, size_t width, size_t height, chunk_reader::SMAP_Chunk& smap_chunk, chunk_reader::APAL_Chunk& apal_chunk, img_info& info)
	{
		size_t header_size = sizeof(chunk_reader::SMAP_Chunk) - sizeof(smap_chunk.data); // Pointer in the SMAP class is size 8 and needs to be deducted.
		size_t smap_size = smap_chunk.ChunkSize() - header_size;

		uint32_t strip_width = 8;
		size_t num_strips = static_cast<size_t>(static_cast<size_t>(floor(static_cast<double>(width / strip_width))));

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
		std::vector< std::vector<std::vector<IndexColor>>> data_blocks;
		for (size_t i = 0; i < strips.size(); i++)
		{
			strip& strip = strips[i];

			std::vector<std::vector<IndexColor>> data_new_block;

			uint8_t code = strip.data[0];

			bool horizontal = true;
			if (code >= 0x03 && code <= 0x12 || code >= 0x22 && code <= 0x26)
				horizontal = false;

			bool he_transparent = code >= 0x22 && code <= 0x30 || code >= 0x54 && code <= 0x80 || code >= 0x8F;

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

			ImageData new_data = ImageData(strip_info.data.size, strip_info.data.data);

			if (!horizontal)
			{
				uint32_t dataIndex = 0;
				for (uint32_t k = 0; k < strip_width; ++k)
				{
					for (uint32_t l = 0; l < static_cast<uint32_t>(height); ++l)
					{
						new_data.data[(l * strip_width) + k] = strip_info.data.data[dataIndex];
						++dataIndex;
					}
				}
			}

			strip_info.data = new_data;
			total_size += strip_info.size;

			for (size_t k = 0; k < height; k++)
			{
				std::vector<IndexColor> new_strip;
				for (size_t b = 0; b < strip_width; b++)
				{
					new_strip.push_back({ strip_info.data.data[(k * strip_width) + b], color });
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

		info.width = width;
		info.height = height;
		info.size = total_size;
		info.channels = 4;

		std::vector<uint8_t> newOut;
		for (size_t i = 0; i < info.size; i++)
		{
			newOut.push_back(apal_chunk.data[final_data[i].index * 3]);
			newOut.push_back(apal_chunk.data[final_data[i].index * 3 + 1]);
			newOut.push_back(apal_chunk.data[final_data[i].index * 3 + 2]);
			if (final_data[i].index == final_data[i].trans_color)
				newOut.push_back(255);
			else
				newOut.push_back(255);
		}

		info.size = newOut.size();
		info.data = ImageData(newOut.size(), newOut.data());

		return true;
	}
}