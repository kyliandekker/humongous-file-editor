#pragma once

#include <string>

namespace HumongousFileEditor
{
	class talk_string : public std::string
	{
	public:
		bool IsTalkString() const;
		size_t GetTalkOffset() const;
		size_t GetTalkOffsetPos() const;
		size_t GetTalkSize() const;
		size_t GetTalkSizePos() const;
	};
}