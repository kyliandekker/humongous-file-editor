#pragma once
#pragma once

#include <string>
#include <vector>
#include "allocators/TempAllocator.h"

namespace HumongousFileEditor
{
	using ImageData = TempAllocator;

	struct img_info
	{
	public:
		ImageData data;
		uint16_t channels = 0;
		size_t size = 0;
		size_t width = 0, height = 0;
		size_t x = 0, y = 0;
	};

	class ImageTab
	{
	protected:
		static bool ReplaceResource(std::string& file_path);
		static bool DecodeHE(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info);
		static bool DecodeBasic(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info);
		static bool DecodeMajmin(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info);
		static bool DecodeRaw(unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info);
	};
};