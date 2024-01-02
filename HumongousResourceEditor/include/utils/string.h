#pragma once

#include <string>

namespace resource_editor
{
	namespace string_extensions
	{
		inline std::string getExtensionFromPath(std::string a_Path, bool a_IncludeDot = false)
		{
			return a_Path.substr(a_Path.find_last_of(".") + (a_IncludeDot ? 0 : 1));
		}

		inline std::string getFileWithoutExtension(std::string a_Path)
		{
			return a_Path.substr(0, a_Path.find_last_of("."));
		}

		inline std::string getFileName(std::string a_Path)
		{
			return a_Path.substr(a_Path.find_last_of("\\") + 1);
		}

		inline std::string getPath(std::string a_Path)
		{
			std::string f = a_Path.substr(0, a_Path.find_last_of("\\"));
			return f.substr(0, a_Path.find_last_of("/"));
		}
	}
}