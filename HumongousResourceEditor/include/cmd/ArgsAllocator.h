#pragma once

#include <vector>
#include <string>

#include "low_level/utils.h"

namespace resource_editor
{
	enum class ArgType
	{
		ArgType_Byte,
		ArgType_Short,
		ArgType_Ref,
		ArgType_Long,
		ArgType_String,
		ArgType_TalkString,
	};

	struct ArgHeader
	{
		ArgHeader(size_t a_Offset, ArgType a_ArgType, unsigned char* a_Data, std::string a_Str, size_t a_Size = 0);

		ArgType m_ArgType;
		size_t m_Size;
		size_t m_Offset;
		std::string m_Str;
		unsigned char* m_Data;
	};

	class ArgsAllocator
	{
	public:
		void AddArg(ArgHeader argHeader);
		size_t Size() const;
		size_t ArgSize() const;
		std::vector<ArgHeader> m_Args;
		ArgHeader& operator [] (size_t i) { return m_Args[i]; };
	};
}