#pragma once

#include "Data.h"
#include <cstdint>

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

namespace resource_editor
{
	class DataStream : public Data
	{
	public:
		DataStream() = default;
		DataStream(void* a_Data, size_t a_Size);
		DataStream(size_t a_Size);
		DataStream(const DataStream& rhs);

		DataStream& operator=(const DataStream& a_Other);
		DataStream& operator=(DataStream& a_Other);

		void Free() override;

		bool Read(void*& a_Data, size_t a_DataSize, size_t a_Size);
		bool Write(void const* a_Data, size_t a_Size);
		bool Seek(size_t a_Offset, size_t a_Whence);
		size_t Tell() const;
	protected:
		size_t m_Pos = 0;
	};
}