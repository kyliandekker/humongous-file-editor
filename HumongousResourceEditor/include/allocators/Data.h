#pragma once

#include <cstdint>

namespace resource_editor
{
	class Data
	{
	public:
		Data() = default;
		Data(size_t a_Size, void* a_Data);
		Data(size_t a_Size);
		Data(const Data& rhs);
		~Data();

		Data& operator=(const Data& a_Other);
		Data& operator=(Data& a_Other);

		const size_t size() const
		{
			return m_Size;
		}

		void* data()
		{
			return m_Data;
		}

		unsigned char operator [] (int i) const { return reinterpret_cast<unsigned char*>(m_Data)[i]; }
		unsigned char& operator [] (int i) { return reinterpret_cast<unsigned char*>(m_Data)[i]; }
	protected:
		void* m_Data = nullptr;
		size_t m_Size = 0;
	};
}