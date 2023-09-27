#pragma once

#include "base/ImageTab.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;
		struct SDAT_Chunk;
		struct HSHD_Chunk;
	}

	class BMAPTab : public ImageTab
	{
	public:
		static bool DecodeSMAP(chunk_reader::FileContainer*& fc, size_t offset, size_t lflf, img_info& info);
		static bool DecodeBMAP(chunk_reader::FileContainer*& fc, size_t offset, img_info& info);
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, size_t lflf, img_info& info);
	};
};