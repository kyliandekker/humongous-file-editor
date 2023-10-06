#include "cmd/talk_string.h"

namespace HumongousFileEditor
{
	bool talk_string::IsTalkString() const
	{
		return (c_str()[0] == 0x7F && c_str()[1] == 'T');
	}
	size_t talk_string::GetTalkOffset() const
	{
		size_t byte_pos_pos = find_first_of("T") + 1;
		size_t comma_pos = find_first_of(",");
		std::string talk_offset = substr(byte_pos_pos, comma_pos - byte_pos_pos);

		return std::stoi(talk_offset);
	}

	size_t talk_string::GetTalkOffsetPos() const
	{
		size_t byte_pos_pos = find_first_of("T") + 1;
		return byte_pos_pos;
	}

	size_t talk_string::GetTalkSize() const
	{
		size_t comma_pos = find_first_of(",");
		size_t bracket_pos = find_first_of("[");
		std::string talk_size = substr(comma_pos + 1, (bracket_pos - 1) - (comma_pos + 1));

		return std::stoi(talk_size);
	}

	size_t talk_string::GetTalkSizePos() const
	{
		size_t comma_pos = find_first_of(",");
		return comma_pos + 1;
	}
}