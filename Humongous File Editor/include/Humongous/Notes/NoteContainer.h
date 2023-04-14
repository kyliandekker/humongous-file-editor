#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cstdint>

namespace uaudio
{
	namespace wave_reader
	{
		class ChunkHeader;
	}
}

namespace HumongousFileEditor
{
	namespace notes
	{
		class Note;

		public class NoteContainer
		{
		public:
			void LoadNotes(std::string path);
			void SaveNotes(std::string path);
			void AddNote(std::string note, uint32_t num);
			System::String^ GetNote(uint32_t num) const;
		private:
			void ThrowError(System::String^ message);
			void getChunk(uaudio::wave_reader::ChunkHeader& chunk, FILE* file);
			std::vector<Note> m_Notes;
		};
		extern NoteContainer noteContainer;
	}
}