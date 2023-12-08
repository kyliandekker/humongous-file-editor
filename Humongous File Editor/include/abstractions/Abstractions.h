#pragma once

#include <Windows.h>
#include <string>

namespace HumongousFileEditor
{
	namespace abstractions
	{
		bool OpenWFile(std::string& path, LPCWSTR filter = L"");
		bool SaveWFile(std::string& path, int* choice = nullptr, LPCWSTR filter = L"");
		bool SaveFolder(std::string& path);
	}
}