#include "Humongous/Notes/NoteContainer.h"

#include <windows.h>

#include "utils/utils.h"
#include "Humongous/Notes/NoteChunks.h"

#include "Humongous/Notes/Note.h"

HumongousFileEditor::notes::NoteContainer HumongousFileEditor::notes::noteContainer;

namespace HumongousFileEditor
{
	namespace notes
	{
		void NoteContainer::LoadNotes(std::string path)
		{
				return;
			FILE* file = nullptr;
			std::string notes_path = path + ".hfe";
			fopen_s(&file, notes_path.c_str(), "rb");
			if (file == nullptr)
			{
			}

			// Go to eof and store the size.
			fseek(file, 0, SEEK_END);
			uint32_t fileSize = ftell(file);
			rewind(file);

			HFE_Chunk hfe_chunk;
			getChunk(hfe_chunk, file);
			if (utils::chunkcmp(hfe_chunk.chunk_id, HFE_CHUNK_ID) != 0)
			{
				ThrowError("Parsing error in note file: Failed loading HFE chunk.");
				return;
			}
			fread(&hfe_chunk.noteCount, sizeof(uint32_t), 1, file);

			if (ftell(file) == fileSize && hfe_chunk.noteCount > 0)
			{
				ThrowError("Parsing error in note file: Note count is higher than 0 but no notes saved.");
				return;
			}

			for (size_t i = 0; i < hfe_chunk.noteCount; i++)
			{
				NOTE_Chunk note_chunk;
				getChunk(note_chunk, file);
				if (utils::chunkcmp(note_chunk.chunk_id, NOTE_CHUNK_ID) != 0)
				{
					ThrowError("Parsing error in note file: Failed loading NOTE chunk.");
					return;
				}
				fread(&note_chunk.num, sizeof(uint32_t), 1, file);
				fread(note_chunk.data, note_chunk.chunkSize - sizeof(NOTE_Chunk), 1, file);

				m_Notes.push_back(Note(std::string(note_chunk.data), note_chunk.num));
			}

			fclose(file);
		}

		void NoteContainer::SaveNotes(std::string path)
		{
			FILE* file = nullptr;
			std::string notes_path = path + ".hfe";
			fopen_s(&file, notes_path.c_str(), "wb");
			if (file == nullptr)
			{
				return;
			}

			HFE_Chunk hfe_chunk;
			memcpy(&hfe_chunk.chunk_id, HFE_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
			fwrite(hfe_chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			hfe_chunk.noteCount = m_Notes.size();
			fwrite(&hfe_chunk.noteCount, sizeof(uint32_t), 1, file);
			hfe_chunk.chunkSize = sizeof(HFE_Chunk);
			for (size_t i = 0; i < m_Notes.size(); i++)
			{
				hfe_chunk.chunkSize += sizeof(NOTE_Chunk) - sizeof(char*);
				hfe_chunk.chunkSize += m_Notes[i].text.length();
			}

			unsigned char chunk_size[sizeof(uint32_t)];

			// Write the chunk size of the tlkb chunk.
			memcpy(chunk_size, &hfe_chunk.chunkSize, sizeof(uint32_t));
			utils::reverseBytes(chunk_size, sizeof(uint32_t));
			fwrite(chunk_size, sizeof(uint32_t), 1, file);

			for (size_t i = 0; i < m_Notes.size(); i++)
			{
				NOTE_Chunk note_chunk;
				memcpy(&note_chunk.chunk_id, NOTE_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
				fwrite(note_chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
				note_chunk.chunkSize = sizeof(NOTE_Chunk) - sizeof(note_chunk.data) + m_Notes[i].text.length();
				memcpy(&chunk_size, &note_chunk.chunkSize, sizeof(uint32_t));
				utils::reverseBytes(chunk_size, sizeof(uint32_t));
				fwrite(&note_chunk.num, sizeof(uint32_t), 1, file);
				fwrite(&chunk_size, sizeof(uint32_t), 1, file);
				fwrite(m_Notes[i].text.c_str(), m_Notes[i].text.length(), 1, file);
			}

			fclose(file);
		}

		void NoteContainer::AddNote(std::string note, uint32_t num)
		{
			// Avoid double notes.
			for (size_t i = 0; i < m_Notes.size(); i++)
			{
				if (m_Notes[i].index == num)
					return;
			}
			m_Notes.push_back(Note(note, num));
		}

		System::String^ NoteContainer::GetNote(uint32_t num) const
		{
			for (size_t i = 0; i < m_Notes.size(); i++)
			{
				if (m_Notes[i].index == num)
					return gcnew System::String(m_Notes[i].text.c_str());
			}
		}

		void NoteContainer::ThrowError(System::String^ message)
		{
			System::Windows::Forms::MessageBox::Show(message, "Note parsing error", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
		}

		void NoteContainer::getChunk(uaudio::wave_reader::ChunkHeader& chunk, FILE* file)
		{
			// Store the chunk size so that it can be reversed (from little to big endian).
			unsigned char chunk_size[sizeof(uint32_t)];
			// Read the chunk id first.
			fread(&chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			fread(&chunk_size, sizeof(uint32_t), 1, file);
			chunk.chunkSize = utils::little_to_big_endian<uint32_t>(chunk_size);
		}
	}
}