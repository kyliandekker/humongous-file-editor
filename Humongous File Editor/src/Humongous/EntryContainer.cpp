#include "Humongous/EntryContainer.h"

#include "Humongous/Decompiler.h"
#include "Humongous/Compiler.h"
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

    Entry* EntryContainer::FindByPos(size_t pos)
    {
		for (size_t i = 0; i < entries.size(); i++)
			if (entries[i]->pos == pos)
				return entries[i];

		return nullptr;
    }

	void EntryContainer::Decompile(std::string path)
	{
		Clear();

		SetFile(path);
		decompiler::decompile();
	}

	void EntryContainer::Compile(std::string path)
	{
		compiler::compile(path);
	}

	void EntryContainer::Crypt(std::string path)
	{
		compiler::crypt(path);
	}

	size_t EntryContainer::size() const
	{
		return entries.size();
	}
}