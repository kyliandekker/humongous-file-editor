#pragma once

#include <Windows.h>
#include <shtypes.h>
#include <string>
#include <vector>
#include <shtypes.h>

namespace HumongousFileEditor
{
	namespace abstractions
	{
		bool OpenWFile(std::string& path, const std::vector<COMDLG_FILTERSPEC>& filters = {});
		bool SaveWFile(std::string& path, int* choice = nullptr, const std::vector<COMDLG_FILTERSPEC>& filters = {});
		bool SaveFolder(std::string& path);
		bool OpenFolder(std::string& path);
	}
}