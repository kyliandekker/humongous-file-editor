#pragma once

#include <corecrt_malloc.h>
#include <cstdint>
#include <utils/TempAllocator.h>

namespace resource_editor
{
	namespace game
	{
		class ImgInfo
		{
		public:
			DataContainer m_Data;
			size_t m_Width = 0, m_Height = 0;
			size_t m_X = 0, m_Y = 0;

			const size_t Size() const
			{
				return m_Data.Size();
			}

			unsigned char* Data()
			{
				return m_Data.Data();
			}

			unsigned char operator [] (int i) const { return m_Data[i]; }
			unsigned char& operator [] (int i) { return m_Data[i]; }
		};
	}
}