#pragma once

#include <vector>
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
		ArgHeader(size_t offset, ArgType argType, size_t size = 0);

		ArgType argType;
		size_t size;
		size_t offset;
	};

	class ArgsAllocator
	{
	public:
		void AddArg(ArgHeader argHeader);
		std::vector<ArgHeader> args;
		size_t Size() const;
		ArgHeader& operator [] (size_t i) { return args[i]; };
	};
}