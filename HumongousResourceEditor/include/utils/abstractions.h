#pragma once

#include <string>
#include <vector>
#include <shtypes.h>

namespace resource_editor
{
	namespace abstractions
	{
		bool PickContainer(std::string& path);
		bool PickFile(std::string& path, const std::vector<COMDLG_FILTERSPEC>& filters = {});
		bool SaveFile(std::string& path, int* choice = nullptr, const std::vector<COMDLG_FILTERSPEC>& filters = {});
		bool SaveFolder(std::string& path);
	}
}