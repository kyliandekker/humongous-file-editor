#pragma once

#include <cstdint>

namespace HumongousFileEditor
{
	namespace BMAP
	{
		public class BMPHead
		{
		public:
			char filehd_type[2] = { 'B', 'M' };
			uint32_t filehd_size = 0;
			uint16_t filehd_reserved1 = 0;
			uint16_t filehd_reserved2 = 0;
			uint32_t filehd_offbits = 0;
		};
		public class BMPHeader
		{
		public:
			uint32_t infohd_size = 0;
			uint32_t infohd_width = 0;
			uint32_t infohd_height = 0;
			uint16_t infohd_planes = 0;
			uint16_t infohd_bitcount = 0;
			uint32_t infohd_compression = 0;
			uint32_t infohd_sizeimage = 0;
			uint32_t infohd_xpelsm = 0;
			uint32_t infohd_ypelsm = 0;
			uint32_t infohd_clrused = 0;
			uint32_t infohd_clrimp = 0;
		};
	}
}