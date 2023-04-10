#pragma once

#include <string>

namespace HumongousFileEditor
{
	public enum class FileType
	{
		FileType_Song,
		FileType_Talkie,
	};

	inline std::string FileTypeToString(FileType fileType)
	{
		switch (fileType)
		{
			case FileType::FileType_Song:
			{
				return "Song";
				break;
			}
			case FileType::FileType_Talkie:
			{
				return "Sound/Talkie";
				break;
			}
		}
		return "Unknown";
	}
}