#pragma once

#include "EntryType.h"
#include <string>

namespace HumongousFileEditor
{
	class Entry
	{
	public:
		EntryType entryType;
		uint32_t pos = 0;
		uint32_t num = 0;

		virtual ~Entry()
		{ }

		/// <summary>
		/// Opens the save dialog window (with dynamic variables).
		/// </summary>
		void OpenSave();
		/// <summary>
		/// Opens the open dialog window (with dynamic variables).
		/// </summary>
		void OpenReplace();
		/// <summary>
		/// Saves the entry data to a file.
		/// </summary>
		/// <param name="path">The path used to save the data to.</param>
		virtual void Save(std::string path) = 0;
		/// <summary>
		/// Returns the extension related to the entry.
		/// </summary>
		/// <returns></returns>
		virtual std::string GetCommonExtension()
		{
			return "";
		}
	protected:
		/// <summary>
		/// Replaces the data of the entry.
		/// </summary>
		/// <param name="path"></param>
		virtual void Replace(std::string path) = 0;
		/// <summary>
		/// Returns the filters related to the entry.
		/// </summary>
		/// <returns></returns>
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
			entryType = EntryType::EntryType_Song;
		}
		virtual ~SongEntry() override
		{
			if (data != nullptr)
			{
				free(data);
			}
		}
		/// <summary>
		/// Saves the entry data to a file.
		/// </summary>
		/// <param name="path">The path used to save the data to.</param>
		virtual void Save(std::string path);
		/// <summary>
		/// Returns the extension related to the entry.
		/// </summary>
		/// <returns></returns>
		virtual std::string GetCommonExtension()
		{
			return ".wav\0";
		}
	protected:
		/// <summary>
		/// Replaces the data of the entry.
		/// </summary>
		/// <param name="path"></param>
		virtual void Replace(std::string path);
		/// <summary>
		/// Returns the filters related to the entry.
		/// </summary>
		/// <returns></returns>
		virtual std::string GetFilter()
		{
			return "Microsoft wave file (*.wav)\0*.wav\0";
		}

	public:
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
			entryType = EntryType::EntryType_Talkie;
		}

		size_t sbng_size = 0;
	};
}