#pragma once

#include <string>

namespace HumongousFileEditor
{
	namespace files
	{
		public enum FileType
		{
			FileType_Unknown,
			FileType_HE0,
			FileType_A,
			FileType_HE2,
			FileType_HE4
		};

		inline std::string getExtensionFromPath(std::string path, bool include_dot = false)
		{
			return path.substr(path.find_last_of(".") + (include_dot ? 0 : 1));
		}

		inline FileType getFileTypeByExtension(std::string path)
		{
			std::string extension = getExtensionFromPath(path);

			if (extension == "HE0")
				return FileType_HE0; 
			else if (extension == "(A)")
				return FileType_A;
			else if (extension == "HE2")
				return FileType_HE2;
			else if (extension == "HE4")
				return FileType_HE4;
			return FileType_Unknown;
		}

		inline std::string getFileWithoutExtension(std::string path)
		{
			return path.substr(0, path.find_last_of("."));
		}

		inline std::string getExtensionByFileType(FileType fileType)
		{
			switch (fileType)
			{
				case FileType::FileType_HE0:
				{
					return ".HE0";
				}
				case FileType::FileType_A:
				{
					return ".(A)";
				}
				case FileType::FileType_HE4:
				{
					return ".HE4";
				}
				case FileType::FileType_HE2:
				{
					return ".HE2";
				}
			}
			return "";
		}

		inline std::string getFileName(std::string path)
		{
			return path.substr(path.find_last_of("\\") + 1);
		}
	}
}