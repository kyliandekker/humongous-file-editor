#pragma once

namespace HumongousFileEditor
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

		unsigned char* data = nullptr;
		size_t size = 0;
	};
}