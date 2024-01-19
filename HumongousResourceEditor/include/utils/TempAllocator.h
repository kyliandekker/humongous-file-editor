#pragma once

#include <cstdint>

namespace resource_editor
{
	class TempAllocator
	{
	public:
		TempAllocator() = default;
		TempAllocator(size_t a_Size, void* a_Data);
		TempAllocator(size_t a_Size);
		TempAllocator(const TempAllocator& rhs);
		~TempAllocator();

		TempAllocator& operator=(const TempAllocator& a_Other);
		TempAllocator& operator=(TempAllocator& a_Other);

		const size_t Size() const
		{
			return m_Size;
		}

		void* Data()
		{
			return m_Data;
		}

		unsigned char operator [] (int i) const { return reinterpret_cast<unsigned char*>(m_Data)[i]; }
		unsigned char& operator [] (int i) { return reinterpret_cast<unsigned char*>(m_Data)[i]; }
	protected:
		void* m_Data = nullptr;
		size_t m_Size = 0;
	};

	using ImageData = TempAllocator;
	using DataContainer = TempAllocator;
}