#pragma once

#include <vector>
#include <string>
#include "FileType.h"

namespace HumongousFileEditor
{
	class Entry;

	public class EntryContainer
	{
	public:
		std::string filePath = "";
		std::string fileName = "";
		FileType fileType = FileType_None;

		void Clear();
		void AddEntry(Entry* entry);
		void SetFile(std::string path);
		Entry* FindByPos(size_t pos);

		void Decompile(std::string path);
		void Compile(std::string path);
		void Crypt(std::string path);

		Entry* operator [] (int i) const { return entries[i]; }
		size_t size() const;
	private:
		std::vector<Entry*> entries;
	};
	extern EntryContainer entryContainer;
}