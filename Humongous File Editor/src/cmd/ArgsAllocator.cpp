#include "cmd/ArgsAllocator.h"
#include <cstdint>

namespace HumongousFileEditor
{
	ArgHeader::ArgHeader(size_t offset, ArgType argType, size_t size)
	{
		this->offset = offset;
		this->argType = argType;
		switch (this->argType)
		{
			case ArgType::ArgType_Byte:
			{
				this->size = sizeof(uint8_t);
				break;
			}
			case ArgType::ArgType_Short:
			case ArgType::ArgType_Ref:
			{
				this->size = sizeof(uint16_t);
				break;
			}
			case ArgType::ArgType_Long:
			{
				this->size = sizeof(uint32_t);
				break;
			}
			case ArgType::ArgType_String:
			case ArgType::ArgType_TalkString:
			{
				this->size = size;
				break;
			}
		}
	}

	void ArgsAllocator::AddArg(ArgHeader argHeader)
	{
		args.push_back(argHeader);
	}

	size_t ArgsAllocator::Size() const
	{
		size_t size = 0;
		for (size_t i = 0; i < args.size(); i++)
			size += args[i].size;
		return size;
	}
}