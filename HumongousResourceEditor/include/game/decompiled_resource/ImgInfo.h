#pragma once

#include <corecrt_malloc.h>
#include <cstdint>
#include "allocators/Data.h"

namespace resource_editor
{
	namespace game
	{
		class ImgInfo
		{
		public:
			Data m_Data;
			size_t m_Width = 0, m_Height = 0;
			size_t m_X = 0, m_Y = 0;
			unsigned char m_FillColor = 0;

			const size_t Size() const
			{
				return m_Data.size();
			}

			unsigned char* Data()
			{
				return reinterpret_cast<unsigned char*>(m_Data.data());
			}
		};
	}
}