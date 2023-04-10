#pragma once

#include "FileType.h"
#include <string>

namespace HumongousFileEditor
{
	class File
	{
	public:
		FileType fileType;
		uint32_t num = 0;

		virtual ~File()
		{ }

		void OpenSave();

		virtual void Save(std::string path) = 0;
		virtual std::string GetCommonExtension()
		{
			return std::to_string(num);
		}
	};

	class SongFile : public File
	{
	public:
		SongFile() : File()
		{
			fileType = FileType::FileType_Song;
		}
		virtual ~SongFile() override
		{
			if (data != nullptr)
			{
				free(data);
			}
		}
		virtual void Save(std::string path);
		virtual std::string GetCommonExtension()
		{
			return std::to_string(num) + ".wav\0";
		}

		unsigned char* data = nullptr;
		uint32_t size = 0;
		uint16_t sample_rate = 0;
	};

	class TalkieFile : public SongFile
	{
	public:
		TalkieFile() : SongFile()
		{
			fileType = FileType::FileType_Talkie;
		}
		virtual void Save(std::string path);

		bool hasSbng = false;
	};
}