#include "functions/FileDecrypter.h"

#include <windows.h>

#include "lowlevel/FileContainer.h"
#include "systems/Logger.h"
#include "lowlevel/utils.h"
#include "HumongousEditorForm.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		bool FileDecrypter::Read(std::string path, std::string savePath)
		{
			chunk_reader::FileContainer fc = chunk_reader::FileContainer(path);
			fc.Decrypt(0x69);

			std::string extension = files::getExtensionByFileType(fc.fileType);

			if (!utils::ends_with(savePath, extension) && !utils::ends_with(savePath, extension))
				savePath += extension;

			FILE* file = nullptr;
			fopen_s(&file, savePath.c_str(), "wb");
			if (file == nullptr)
			{
				LOGF(logger::LOGSEVERITY_ERROR, "Cannot save file \"%s\".", savePath.c_str());
				return false;
			}

			fwrite(fc.data, fc.size, 1, file);
			fclose(file);

			return true;
		}
	}
}