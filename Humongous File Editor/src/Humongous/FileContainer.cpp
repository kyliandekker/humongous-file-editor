#include "Humongous/FileContainer.h"

hfe::FileContainer hfe::fileContainer;

namespace hfe
{
	void FileContainer::Clear()
	{
		if (files.size() == 0)
			return;

		for (size_t i = files.size() - 1; i > 0; i--)
			delete files[i];
		files.clear();
	}
}