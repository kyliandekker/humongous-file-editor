#include "game/compilers/Decrypter.h"
#include "utils/abstractions.h"
#include "project/Resource.h"
#include "utils/string.h"

namespace resource_editor
{
	namespace game
	{
		bool Decrypter::Decrypt(project::Resource& a_Resource)
		{
			std::string path;

			if (!abstractions::SaveFile(path, nullptr, abstractions::GetFilters(a_Resource.m_ResourceType)))
			{
				return false;
			}

			std::string extension = string_extensions::getExtensionFromPath(a_Resource.m_Path);
			if (!string_extensions::ends_with(path, extension))
			{
				path += "." + extension;
			}

			chunk_reader::FileContainer fileContainer;
			fileContainer.Open(a_Resource.m_Path);
			fileContainer.Decrypt();

			FILE* file = nullptr;
			fopen_s(&file, path.c_str(), "wb");
			if (!file)
			{
				return false;
			}

			fwrite(fileContainer.data(), fileContainer.size(), 1, file);

			fclose(file);

			return true;
		}
	}
}