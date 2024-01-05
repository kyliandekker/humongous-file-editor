#pragma once

namespace resource_editor
{
	class TempAllocator
	{
	public:
		TempAllocator() = default;
		TempAllocator(size_t size, unsigned char* data);
		TempAllocator(size_t size);
		TempAllocator(const TempAllocator& rhs);
		~TempAllocator();

		TempAllocator& operator=(const TempAllocator& other);
		TempAllocator& operator=(TempAllocator& other);

		const size_t Size() const
		{
			return m_Size;
		}

		unsigned char* Data()
		{
			return m_Data;
		}

		unsigned char operator [] (int i) const { return m_Data[i]; }
		unsigned char& operator [] (int i) { return m_Data[i]; }
	private:
		unsigned char* m_Data = nullptr;
		size_t m_Size = 0;
	};

	using ImageData = TempAllocator;
	using DataContainer = TempAllocator;
}