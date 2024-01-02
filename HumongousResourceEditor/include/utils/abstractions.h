#pragma once

#include <string>
#include <vector>
#include <shtypes.h>

namespace resource_editor
{
	namespace abstractions
	{
		bool PickContainer(std::string& a_Path);
		bool PickFile(std::string& a_Path, const std::vector<COMDLG_FILTERSPEC>& a_Filters = {});
		bool SaveFile(std::string& a_Path, int* a_Choice = nullptr, const std::vector<COMDLG_FILTERSPEC>& a_Filters = {});
		bool SaveFolder(std::string& a_Path);
	}
}