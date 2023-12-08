#include "functions/types/base/ImageTab.h"

#include <vector>

namespace HumongousFileEditor
{
	std::vector<uint8_t> create_bitstream(unsigned char* data, size_t length)
	{
		std::vector<uint8_t> bits;
		for (size_t i = 0; i < length; ++i)
		{
			char c = data[i];
			for (int j = 0; j < 8; j++)
			{
				bits.push_back((c >> j) & 1);
			}
		}
		return bits;
	}

	uint8_t collect_bits(int& pos, std::vector<uint8_t>& bitstream, int count)
	{
		int result = 0;
		for (int i = 0; i < count; i++)
		{
			result |= bitstream[pos++] << i;
		}

		return static_cast<uint8_t>(result);
	}

	bool ImageTab::ReplaceResource(std::string&)
	{
		return false;
	}

	bool ImageTab::DecodeHE(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool, img_info& info)
	{
		std::vector<uint8_t> out;

		info.width = width;
		info.height = height;

		unsigned char color = fill_color;

		size_t num_pixels = info.width * info.height;

		if (data_size == 0)
		{
			for (size_t i = 0; i < num_pixels; i++)
			{
				out.push_back(color % 256);
			}
		}
		else
		{
			std::vector<int8_t> delta_color = { -4, -3, -2, -1, 1, 2, 3, 4 };

			std::vector<uint8_t> bits = create_bitstream(data, data_size);

			out.push_back(color % 256);

			int pos = 0;
			while (out.size() < num_pixels)
			{
				if (bits[pos++] == 1)
				{
					if (bits[pos++] == 1)
					{
						uint8_t bitc = collect_bits(pos, bits, 3);
						color += delta_color[bitc];
					}
					else
					{
						color = collect_bits(pos, bits, palen);
					}
				}
				out.push_back(color % 256);
			};
		}

		info.size = out.size();
		info.data = ImageData(out.size(), out.data());
		return true;
	}

	bool ImageTab::DecodeBasic(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info)
	{
		info.width = width;
		info.height = height;

		unsigned char color = fill_color;

		std::vector<uint8_t> bits = create_bitstream(data, data_size);

		std::vector<uint8_t> out;

		info.channels = 3;
		if (transparent)
		{
			info.channels = 4;
		}

		out.push_back(color % 256);

		size_t num_pixels = info.width * info.height;

		int8_t sub = 1;
		int pos = 0;
		while (out.size() < num_pixels)
		{
			if (bits[pos++] == 1)
			{
				if (bits[pos++] == 1)
				{
					if (bits[pos++] == 1)
					{
						sub = -sub;
					}
					color -= sub;
				}
				else
				{
					color = collect_bits(pos, bits, palen);
					sub = 1;
				}
			}
			out.push_back(color % 256);
		};

		info.size = out.size();
		info.data = ImageData(out.size(), out.data());
		return true;
	}

	bool ImageTab::DecodeMajmin(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info)
	{
		info.width = width;
		info.height = height;

		unsigned char color = fill_color;

		std::vector<uint8_t> bits = create_bitstream(data, data_size);
		std::vector<uint8_t> out;

		info.channels = 3;
		if (transparent)
		{
			info.channels = 4;
		}

		out.push_back(color % 256);

		size_t num_pixels = info.width * info.height;

		int pos = 0;
		while (out.size() < num_pixels)
		{
			if (bits[pos++] == 1)
			{
				if (bits[pos++] == 1)
				{
					uint8_t shift = collect_bits(pos, bits, 3) - 4;
					if (shift != 0)
					{
						color += shift;
					}
					else
					{
						uint8_t ln = collect_bits(pos, bits, 8) - 1;
						for (size_t i = 0; i < ln; i++)
						{
							out.push_back((color % 256));
						}
					}
				}
				else
				{
					color = collect_bits(pos, bits, palen);
				}
			}
			out.push_back(color % 256);
		};

		info.size = out.size();
		info.data = ImageData(out.size(), out.data());
		return true;
	}

	bool ImageTab::DecodeRaw(unsigned char* data, size_t, size_t width, size_t height, int, bool, img_info& info)
	{
		info.size = width * height;
		info.data = ImageData(info.size, data);
		return true;
	}
}