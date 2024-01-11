#include "cmd/ArgsAllocator.h"
#include <cstdint>

namespace resource_editor
{
	ArgHeader::ArgHeader(size_t a_Offset, ArgType a_ArgType, unsigned char* a_Data, std::string a_Str, size_t a_Size)
	{
		this->m_Offset = a_Offset;
		this->m_ArgType = a_ArgType;
		this->m_Str = a_Str;
		this->m_Data = a_Data;
		switch (this->m_ArgType)
		{
			case ArgType::ArgType_Byte:
			{
				this->m_Size = sizeof(uint8_t);
				break;
			}
			case ArgType::ArgType_Short:
			case ArgType::ArgType_Ref:
			{
				this->m_Size = sizeof(uint16_t);
				break;
			}
			case ArgType::ArgType_Long:
			{
				this->m_Size = sizeof(uint32_t);
				break;
			}
			case ArgType::ArgType_String:
			case ArgType::ArgType_TalkString:
			{
				this->m_Size = a_Size;
				break;
			}
		}
	}

	void ArgsAllocator::AddArg(ArgHeader argHeader)
	{
		m_Args.push_back(argHeader);
	}

	size_t ArgsAllocator::Size() const
	{
		size_t size = 0;
		for (size_t i = 0; i < m_Args.size(); i++)
		{
			size += m_Args[i].m_Size;
		}
		return size;
	}

	size_t ArgsAllocator::ArgSize() const
	{
		return m_Args.size();
	}
}