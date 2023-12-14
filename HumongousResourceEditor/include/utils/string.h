#pragma once

#include <string>

namespace resource_editor
{
	namespace string_extensions
	{
		inline std::string getExtensionFromPath(std::string path, bool include_dot = false)
		{
			return path.substr(path.find_last_of(".") + (include_dot ? 0 : 1));
		}

		inline std::string getFileWithoutExtension(std::string path)
		{
			return path.substr(0, path.find_last_of("."));
		}

		inline std::string getFileName(std::string path)
		{
			return path.substr(path.find_last_of("\\") + 1);
		}

		inline std::string getPath(std::string path)
		{
			std::string f = path.substr(0, path.find_last_of("\\"));
			return f.substr(0, path.find_last_of("/"));
		}
	}
}