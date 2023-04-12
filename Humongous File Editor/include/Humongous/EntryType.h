#pragma once

#include <string>

namespace HumongousFileEditor
{
	public enum class EntryType
	{
		EntryType_Song,
		EntryType_Talkie,
	};

	inline std::string EntryTypeToString(EntryType fileType)
	{
		switch (fileType)
		{
			case EntryType::EntryType_Song:
			{
				return "Song";
				break;
			}
			case EntryType::EntryType_Talkie:
			{
				return "Sound/Talkie";
				break;
			}
		}
		return "Unknown";
	}
}