#pragma once

#include "EntryType.h"
#include <string>

namespace HumongousFileEditor
{
	class Entry
	{
	public:
		EntryType fileType;
		uint32_t num = 0;

		virtual ~Entry()
		{ }

		void OpenSave();
		void OpenReplace();

		virtual void Save(std::string path) = 0;
		virtual void Replace(std::string path) = 0;
		virtual std::string GetCommonExtension()
		{
			return "";
		}
		virtual std::string GetFilter()
		{
			return "All files (*.*)\
					\0*.*\0";
		}
	};

	class SongEntry : public Entry
	{
	public:
		SongEntry() : Entry()
		{
			fileType = EntryType::EntryType_Song;
		}
		virtual ~SongEntry() override
		{
			if (data != nullptr)
			{
				free(data);
			}
		}
		virtual void Save(std::string path);
		virtual void Replace(std::string path);
		virtual std::string GetCommonExtension()
		{
			return ".wav\0";
		}
		virtual std::string GetFilter()
		{
			return "Microsoft wave file (*.wav)\0*.wav\0";
		}

		unsigned char* data = nullptr;
		uint32_t size = 0;
		uint16_t sample_rate = 0;
		bool hasSGEN = false;
	};

	class TalkieEntry : public SongEntry
	{
	public:
		TalkieEntry() : SongEntry()
		{
			fileType = EntryType::EntryType_Talkie;
		}
		virtual void Save(std::string path);

		bool hasSbng = false;
	};
}