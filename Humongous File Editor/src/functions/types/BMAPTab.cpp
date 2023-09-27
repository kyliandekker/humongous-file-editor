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

	bool BMAPTab::DecodeSMAP(chunk_reader::FileContainer*& fc, size_t offset, size_t lflf, img_info& info)
	{
		// Get IMHD chunk for width and height of image.
		size_t imhd_offset = chunk_reader::ChunkFunctions::GetOffsetChunk(fc, offset, { chunk_reader::IMHD_CHUNK_ID });
		if (imhd_offset == -1)
			return false;

		chunk_reader::IMHD_Chunk imhd_chunk;
		memcpy(&imhd_chunk, utils::add(fc->data, imhd_offset), sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data));
		imhd_chunk.data = utils::add(fc->data, imhd_offset + (sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data)));

		// Get SMAP chunk for raw data.
		size_t smap_offset = chunk_reader::ChunkFunctions::GetOffsetChunk(fc, offset, { chunk_reader::SMAP_CHUNK_ID });
		if (smap_offset == -1)
			return false;

		chunk_reader::SMAP_Chunk smap_chunk;
		size_t header_size = sizeof(chunk_reader::SMAP_Chunk) - sizeof(smap_chunk.data); // Pointer in the SMAP class is size 8 and needs to be deducted.
		memcpy(&smap_chunk, utils::add(fc->data, smap_offset), header_size);
		smap_chunk.data = utils::add(fc->data, smap_offset + header_size);
		size_t smap_size = smap_chunk.ChunkSize() - header_size;

		//------------------------------------

		uint32_t strip_width = 8;
		size_t num_strips = static_cast<size_t>(floor(imhd_chunk.width / strip_width));

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
				if (!DecodeMajmin(color, utils::add(strip.data, 2), strip.size - 2, strip_width, imhd_chunk.height, palen, he_transparent, strip_info))
					return false;
			}
			else if (code >= 0x0E && code <= 0x30)
			{
				if (!DecodeBasic(color, utils::add(strip.data, 2), strip.size - 2, strip_width, imhd_chunk.height, palen, he_transparent, strip_info))
					return false;
			}
			else if (code >= 0x86 && code <= 0x94)
			{
				if (!DecodeHE(color, utils::add(strip.data, 2), strip.size - 2, strip_width, imhd_chunk.height, palen, he_transparent, strip_info))
					return false;
			}
			else if (code >= 0x01 && code <= 0x95)
			{
				if (!DecodeRaw(strip.data, strip.size, strip_width, imhd_chunk.height, palen, he_transparent, strip_info))
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
					for (int j = 0; j < imhd_chunk.height; ++j)
					{
						data[(j * strip_width) + k] = strip_info.data[dataIndex];
						++dataIndex;
					}
				}
			}

			free(strip_info.data);
			strip_info.data = data;
			total_size += strip_info.size;

			for (size_t k = 0; k < imhd_chunk.height; k++)
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

		info.width = imhd_chunk.width;
		info.height = imhd_chunk.height;
		info.data = total_data;
		info.size = total_size;
		info.channels = 4;

		size_t apal_offset = chunk_reader::ChunkFunctions::GetOffsetChunk(fc, lflf, { chunk_reader::APAL_CHUNK_ID });
		if (apal_offset == -1)
			return false;

		chunk_reader::APAL_Chunk apal_chunk;
		header_size = sizeof(chunk_reader::APAL_Chunk) - sizeof(apal_chunk.data);
		memcpy(&apal_chunk, utils::add(fc->data, apal_offset), header_size);
		size_t apal_size = apal_chunk.ChunkSize() - header_size;
		apal_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, apal_offset + header_size));

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

	bool BMAPTab::DecodeBMAP(chunk_reader::FileContainer*& fc, size_t offset, img_info& info)
	{
		// Get RMHD chunk for width and height of image.
		size_t rmhd_offset = chunk_reader::ChunkFunctions::GetOffsetChunk(fc, offset, { chunk_reader::RMHD_CHUNK_ID });
		if (rmhd_offset == -1)
			return false;

		chunk_reader::RMHD_Chunk rmhd_chunk;
		memcpy(&rmhd_chunk, utils::add(fc->data, rmhd_offset), sizeof(chunk_reader::RMHD_Chunk));

		// Get BMAP chunk for raw data.
		size_t bmap_offset = chunk_reader::ChunkFunctions::GetOffsetChunk(fc, offset, { chunk_reader::BMAP_CHUNK_ID });
		if (bmap_offset == -1)
			return false;

		chunk_reader::BMAP_Chunk bmap_chunk;
		size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
		memcpy(&bmap_chunk, utils::add(fc->data, bmap_offset), header_size);
		bmap_chunk.data = utils::add(fc->data, bmap_offset + header_size);
		size_t bmap_size = bmap_chunk.ChunkSize() - header_size;

		// Get TRNS chunk for transparency settings.
		size_t trns_offset = chunk_reader::ChunkFunctions::GetOffsetChunk(fc, offset, { chunk_reader::TRNS_CHUNK_ID });
		if (trns_offset == -1)
			return false;

		chunk_reader::TRNS_Chunk trns_chunk;
		memcpy(&trns_chunk, utils::add(fc->data, trns_offset), sizeof(chunk_reader::TRNS_Chunk));

		size_t apal_offset = chunk_reader::ChunkFunctions::GetOffsetChunk(fc, offset, { chunk_reader::APAL_CHUNK_ID });
		if (apal_offset == -1)
			return false;

		chunk_reader::APAL_Chunk apal_chunk;
		header_size = sizeof(chunk_reader::APAL_Chunk) - sizeof(apal_chunk.data);
		memcpy(&apal_chunk, utils::add(fc->data, apal_offset), header_size);
		size_t apal_size = apal_chunk.ChunkSize() - header_size;
		apal_chunk.data = utils::add(fc->data, apal_offset + header_size);

		int palen = bmap_chunk.encoding % 10;

		bool he = bmap_chunk.encoding >= 0x86 && bmap_chunk.encoding <= 0x8A;
		bool he_transparent = bmap_chunk.encoding >= 0x90 && bmap_chunk.encoding <= 0x94;

		if (!DecodeHE(bmap_chunk.fill_color, bmap_chunk.data, bmap_size, rmhd_chunk.width, rmhd_chunk.height, palen, he_transparent, info))
			return false;

		std::vector<uint8_t> newOut;
		for (size_t i = 0; i < info.size; i++)
		{
			newOut.push_back(apal_chunk.data[info.data[i] * 3]);
			newOut.push_back(apal_chunk.data[info.data[i] * 3 + 1]);
			newOut.push_back(apal_chunk.data[info.data[i] * 3 + 2]);
			if (he_transparent)
			{
				if (info.data[i] == bmap_chunk.fill_color)
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

	bool BMAPTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, size_t lflf, img_info& info)
	{
		size_t type_offset = chunk_reader::ChunkFunctions::GetOffsetChunk(fc, offset, {
			chunk_reader::BMAP_CHUNK_ID,
			chunk_reader::SMAP_CHUNK_ID,
			});
		if (type_offset == -1)
			return false;

		chunk_reader::ChunkInfo typeChunk = fc->GetChunkInfo(type_offset);
		if (utils::chunkcmp(typeChunk.chunk_id, chunk_reader::BMAP_CHUNK_ID) == 0)
			return DecodeBMAP(fc, offset, info);
		else if (utils::chunkcmp(typeChunk.chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
			return DecodeSMAP(fc, offset, lflf, info);

		return false;
	}
}