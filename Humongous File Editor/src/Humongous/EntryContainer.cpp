#include "Humongous/EntryContainer.h"

#include "Humongous/Decompiler.h"
#include "Humongous/Entry.h"

HumongousFileEditor::EntryContainer HumongousFileEditor::entryContainer;

namespace HumongousFileEditor
{
	void EntryContainer::Clear()
	{
		if (entries.size() == 0)
			return;

		for (size_t i = entries.size() - 1; i > 0; i--)
			delete entries[i];
		entries.clear();
	}

	void EntryContainer::AddEntry(Entry* entry)
	{
		entry->num = entries.size();
		entries.push_back(entry);
	}

	void EntryContainer::SetFile(std::string path)
	{
		filePath = path;
		fileName = path.substr(path.find_last_of("/") + 1);
		if (fileName.empty())
			fileName = path.substr(path.find_last_of("\\") + 1);
		fileType = getFileTypeByExtension(path);
	}

	void EntryContainer::Decompile(std::string path)
	{
		Clear();

		SetFile(path);
		decompiler::decompile(path);
	}

	void EntryContainer::Compile(std::string path)
	{
	}

	size_t EntryContainer::size() const
	{
		return entries.size();
	}
}