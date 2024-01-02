#pragma once

#include <corecrt_malloc.h>
#include <cstdint>

namespace resource_editor
{
	namespace game
	{
		class ImgInfo
		{
		public:
			~ImgInfo()
			{
				free(m_Data);
			}
			unsigned char* m_Data = nullptr;
			uint16_t m_Channels = 0;
			size_t m_Size = 0;
			size_t m_Width = 0, m_Height = 0;
			size_t m_X = 0, m_Y = 0;
		};
	}
}