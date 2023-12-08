#pragma once

#include <vector>

#include "base/SoundTab.h"
#include "cmd/ArgsAllocator.h"
#include "cmd/talk_instruction.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;
	}

	class SCRPTab : public SoundTab
	{
	public:
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, std::vector<instruction>& instructions);
	};
};