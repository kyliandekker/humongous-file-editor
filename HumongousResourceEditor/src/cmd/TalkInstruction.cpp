#include "cmd/TalkInstruction.h"

namespace resource_editor
{
    TalkString::TalkString(void* a_Data)
    {
		m_Data = a_Data;
    }

    bool TalkString::IsTalkString() const
	{
		// Every talk instruction starts with byte code 127 and the letter T for TALK.
		return (data()[1] == 0x7F && data()[2] == 0x54);
	}

	size_t TalkString::GetTalkOffset() const
	{
		return std::stoi(GetTalkOffsetString());
	}

	std::string TalkString::GetTalkOffsetString() const
	{
		// We get the pos of the offset and get the data until the comma.
		const size_t start_pos = GetTalkOffsetPos();
		std::string instruction = std::string(reinterpret_cast<const char*>(data()));
		const size_t end_pos = instruction.find_first_of(",");
		const std::string talk_offset = instruction.substr(start_pos, end_pos - start_pos);

		return talk_offset;
	}

	size_t TalkString::GetTalkOffsetPos() const
	{
		// We find the position of the T and add 1 to get the offset.
		std::string instruction = std::string(reinterpret_cast<const char*>(data()));
		const size_t start_pos = 3;
		return start_pos;
	}

	size_t TalkString::GetTalkSize() const
	{
		// We find the position of the comma and get the data until the 0x7F byte (Which is before the '[').
		std::string instruction = std::string(reinterpret_cast<const char*>(data()));
		const size_t start_pos = instruction.find_first_of(",") + 1;
		const size_t end_pos = instruction.find_first_of("[") - 1;
		const std::string talk_size = instruction.substr(start_pos, end_pos - start_pos);

		return std::stoi(talk_size);
	}

	size_t TalkString::GetTalkSizePos() const
	{
		std::string instruction = std::string(reinterpret_cast<const char*>(data()));
		const size_t start_pos = instruction.find_first_of(",") + 1;
		return start_pos;
	}

	unsigned char* TalkString::data() const
	{
		return reinterpret_cast<unsigned char*>(m_Data);
	}
}