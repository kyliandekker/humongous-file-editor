#pragma once

#include <vector>
#include <string>

#include "lowlevel/utils.h"

namespace HumongousFileEditor
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
		ArgHeader(size_t offset, ArgType argType, std::string str, size_t size = 0);

		ArgType argType;
		size_t size;
		size_t offset;
		std::string str;
	};

	class ArgsAllocator
	{
	public:
		void AddArg(ArgHeader argHeader);
		size_t Size() const;
        size_t ArgSize() const;
		std::vector<ArgHeader> args;
		ArgHeader& operator [] (size_t i) { return args[i]; };
	};
}