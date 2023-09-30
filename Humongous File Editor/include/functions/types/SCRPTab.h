#pragma once

#include <vector>

#include "base/SoundTab.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;
	}

	struct instruction
	{
		uint8_t code;
		std::string name;
		size_t offset;
		size_t size;
		std::vector<std::string> data_str;
	};

	class SCRPTab : public SoundTab
	{
	public:
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, std::vector<instruction>& instructions);
	};
};