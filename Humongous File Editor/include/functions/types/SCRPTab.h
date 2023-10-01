#pragma once

#include <vector>

#include "base/SoundTab.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;
	}

	struct talk_instruction
	{
		uint8_t code;
		std::string name;

		size_t scrp_offset;
		size_t scrp_size;
		size_t offset_in_scrp_chunk;
		size_t abs_offset;

		size_t talk_offset;
		size_t talk_offset_pos;
		size_t talk_size;
		size_t talk_size_pos;

		std::string full_str;
	};

	class SCRPTab : public SoundTab
	{
	public:
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, std::vector<talk_instruction>& instructions);
	};
};