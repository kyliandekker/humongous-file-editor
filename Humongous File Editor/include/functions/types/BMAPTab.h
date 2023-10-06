#pragma once

#include "base/ImageTab.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;
		struct SDAT_Chunk;
		struct HSHD_Chunk;
		struct OBIM_Chunk;
		struct SMAP_Chunk;
		struct BMAP_Chunk;
		struct APAL_Chunk;
	}

	class BMAPTab : public ImageTab
	{
	public:
		static bool GetDataBMAP(chunk_reader::FileContainer*& fc, chunk_reader::BMAP_Chunk& bmap_chunk, chunk_reader::APAL_Chunk& apal_chunk, uint8_t fill_color, size_t width, size_t height, img_info& info);
		static bool GetDataSMAP(chunk_reader::FileContainer*& fc, chunk_reader::OBIM_Chunk& obim_chunk, size_t width, size_t height, chunk_reader::SMAP_Chunk& smap_chunk, chunk_reader::APAL_Chunk& apal_chunk, img_info& info);

		static bool ReplacePalette(chunk_reader::FileContainer*& fc, size_t offset);
		static bool ReplaceBmap(chunk_reader::FileContainer*& fc, size_t offset);
	};
};