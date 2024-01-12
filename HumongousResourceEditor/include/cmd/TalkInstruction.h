#pragma once

#include <string>

namespace resource_editor
{
	class TalkString : public std::string
	{
	public:
		bool IsTalkString() const;
		size_t GetTalkOffset() const;
		std::string GetTalkOffsetString() const;
		size_t GetTalkOffsetPos() const;
		size_t GetTalkSize() const;
		size_t GetTalkSizePos() const;
	};
}