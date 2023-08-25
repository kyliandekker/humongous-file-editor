#pragma once

#include <string>

namespace HumongousFileEditor
{
	namespace files
	{
		public enum FileType
		{
			FileType_Unknown,
			FileType_A,
			FileType_HE2,
			FileType_HE4,
			FileType_HE0
		};

		inline FileType getFileTypeByExtension(std::string path)
		{
			std::string extension = path.substr(path.find_last_of(".") + 1);

			if (extension == "(A)" || extension == "(a)")
				return FileType_A;
			else if (extension == "HE0" || extension == "he0")
				return FileType_HE0;
			else if (extension == "HE2" || extension == "he2")
				return FileType_HE2;
			else if (extension == "HE4" || extension == "he4")
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
					return ".(a)";
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
		}
	}
}