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
			const std::vector<COMDLG_FILTERSPEC> possible_filters =
			{
				{ L"", L"" },
				{ L"Humongous Index Files (*.HE0)", L"*.HE0" },
				{ L"Humongous Talk Files (*.HE2)", L"*.HE2" },
				{ L"Humongous Song Files (*.HE4)", L"*.HE4" },
				{ L"", L"" },
				{ L"Humongous Resource Files (*.(A))", L"*.(A)" },
			};
			const std::vector<COMDLG_FILTERSPEC> filters =
			{
				possible_filters[(int)a_Resource.m_ResourceType]
			};

			if (!abstractions::SaveFile(path, nullptr, filters))
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