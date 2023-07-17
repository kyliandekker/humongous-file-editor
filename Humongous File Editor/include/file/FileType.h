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
		};

		inline FileType getFileTypeByExtension(std::string path)
		{
			std::string extension = path.substr(path.find_last_of(".") + 1);

			FileType i = FileType_Unknown;
			if (extension == "HE4" || extension == "he4")
				i = FileType_HE4;
			else if (extension == "HE2" || extension == "he2")
				i = FileType_HE2;
			else if (extension == "(A)" || extension == "(a)")
				i = FileType_A;
			return i;
		}
	}
}