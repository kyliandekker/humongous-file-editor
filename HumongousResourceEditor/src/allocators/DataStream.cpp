#include "allocators/DataStream.h"
#include "low_level/Utils.h"

namespace resource_editor
{
    DataStream::DataStream(size_t a_Size, void* a_Data) : Data(a_Size, a_Data)
    { }

	DataStream::DataStream(size_t a_Size) : Data(a_Size)
	{ }

	DataStream::DataStream(const DataStream& rhs) : Data(rhs)
	{ 
		m_Pos = rhs.m_Pos;
	}

	DataStream& DataStream::operator=(const DataStream& a_Other)
	{
		if (&a_Other != this)
		{
			Data::operator=(a_Other);
			m_Pos = a_Other.m_Pos;
		}
		return *this;
	}

	DataStream& DataStream::operator=(DataStream& a_Other)
	{
		if (&a_Other != this)
		{
			Data::operator=(a_Other);
			m_Pos = a_Other.m_Pos;
		}
		return *this;
	}

    bool DataStream::Read(void*& a_Data, size_t a_DataSize, size_t a_Size)
	{
		if (m_Pos == m_Size || m_Pos + a_Size > m_Size)
		{
			return false;
		}

		if (a_DataSize < a_Size)
		{
			return false;
		}

		memcpy(a_Data, low_level::utils::add(m_Data, m_Pos), a_Size);
		Seek(a_Size, SEEK_CUR);
		return true;
	}

    bool DataStream::Write(void* a_Data, size_t a_Size)
    {
		if (m_Pos == m_Size || m_Pos + a_Size > m_Size)
		{
			return false;
		}

		memcpy(low_level::utils::add(m_Data, m_Pos), a_Data, a_Size);
		Seek(a_Size, SEEK_CUR);
		return true;
    }

	bool DataStream::Seek(size_t a_Offset, size_t a_Whence)
	{
		switch (a_Whence)
		{
			case SEEK_SET:
			{
				m_Pos = a_Offset;
				return true;
			}
			case SEEK_CUR:
			{
				m_Pos += a_Offset;
				return true;
			}
			case SEEK_END:
			default:
			{
				m_Pos = m_Size - a_Offset;
				return true;
			}
		}
		return false;
	}

	bool DataStream::Save()
	{
		FILE* file;
		fopen_s(&file, "D:/test.txt", "wb");
		fwrite(m_Data, m_Size, 1, file);
		fclose(file);
		return false;
	}

	size_t DataStream::Tell() const
	{
		return m_Pos;
	}
}