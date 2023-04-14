#include "Humongous/Compiler.h"

#include <uaudio_wave_reader/ChunkHeader.h>
#include <windows.h>
#include <WinBase.h>

#include "Humongous/EntryContainer.h"
#include "Humongous/Entry.h"
#include "Humongous/HumongousChunks.h"
#include "Utils/utils.h"

// TODO: Throw parsing errosr when chunks have the wrong name, size or other inconsistencies. This stuff should be really stern about parsing just like the games.
namespace HumongousFileEditor
{
	namespace compiler
	{
		// TODO: This works, but the (a) and HE0 file still reference positions from the old file.
		/*
			* So there is a special order here that matters for the TALKIE file.
			* It opens with a TALKIE chunk.
			* After that, there will be series of TALK/SBNG/SDAT chunks in that order.
			* This makes it complicated to determine the exact size and positions without doing a bunch of calculations first.
		*/
        int compileHE2(std::string path)
        {
			FILE* file = nullptr;

			// Open the file.
			fopen_s(&file, path.c_str(), "wb");
			if (file == nullptr)
			{
				System::Windows::Forms::MessageBox::Show("Cannot write to file.", "Compilation failed", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
				return 1;
			}

			// Add root chunk.
			TLKB_Chunk tlkb_chunk;
			memcpy(tlkb_chunk.chunk_id, TLKB_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
			fwrite(&tlkb_chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			// The size of the tlkb chunk will be changed later on when all the other chunks are constructed.
			tlkb_chunk.chunkSize = sizeof(TLKB_Chunk);
			
			// Go through all entries, see if they are song entries and if they have sgens.
			std::vector<TalkieEntry*> talkies;
			for (size_t i = 0; i < entryContainer.size(); i++)
			{
				if (entryContainer[i]->fileType == EntryType::EntryType_Talkie)
				{
					TalkieEntry* song = static_cast<TalkieEntry*>(entryContainer[i]);
					talkies.push_back(song);
				}
			}

			// Calculate the root chunk size.
			for (size_t i = 0; i < talkies.size(); i++)
			{
				tlkb_chunk.chunkSize += sizeof(TALK_Chunk);
				tlkb_chunk.chunkSize += sizeof(HSHD_Chunk);
				tlkb_chunk.chunkSize += talkies[i]->sbng_size;
				tlkb_chunk.chunkSize += sizeof(SDAT_Chunk) - sizeof(unsigned char*);
				tlkb_chunk.chunkSize += talkies[i]->size;
			}

			unsigned char chunk_size[sizeof(uint32_t)];

			// Write the chunk size of the tlkb chunk.
			memcpy(&chunk_size, &tlkb_chunk.chunkSize, sizeof(uint32_t));
			utils::reverseBytes(chunk_size, sizeof(uint32_t));
			fwrite(chunk_size, sizeof(uint32_t), 1, file);

			for (size_t i = 0; i < talkies.size(); i++)
			{
				TALK_Chunk talk_chunk;
				memcpy(talk_chunk.chunk_id, TALK_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
				fwrite(&talk_chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
				talk_chunk.chunkSize = 
					sizeof(TALK_Chunk) +
					sizeof(HSHD_Chunk) + 
					sizeof(SDAT_Chunk) - sizeof(unsigned char*) +
					talkies[i]->size;
				memcpy(&chunk_size, &talk_chunk.chunkSize, sizeof(uint32_t));
				utils::reverseBytes(chunk_size, sizeof(uint32_t));
				fwrite(chunk_size, sizeof(uint32_t), 1, file);

				HSHD_Chunk hshd_chunk;
				memcpy(hshd_chunk.chunk_id, HSHD_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
				fwrite(&hshd_chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
				hshd_chunk.chunkSize = sizeof(HSHD_Chunk);
				hshd_chunk.sample_rate = talkies[i]->sample_rate;
				memcpy(&chunk_size, &hshd_chunk.chunkSize, sizeof(uint32_t));
				utils::reverseBytes(chunk_size, sizeof(uint32_t));
				fwrite(chunk_size, sizeof(uint32_t), 1, file);
				fwrite(utils::add(&hshd_chunk, sizeof(uaudio::wave_reader::ChunkHeader)), hshd_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1, file);

				if (talkies[i]->sbng_size > 0)
				{
					SBNG_Chunk sbng_chunk;
					memcpy(sbng_chunk.chunk_id, SBNG_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
					fwrite(&sbng_chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
					sbng_chunk.chunkSize = talkies[i]->sbng_size;
					memcpy(&chunk_size, &sbng_chunk.chunkSize, sizeof(uint32_t));
					utils::reverseBytes(chunk_size, sizeof(uint32_t));
					fwrite(chunk_size, sizeof(uint32_t), 1, file);
					fseek(file, sbng_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), SEEK_CUR);
				}

				SDAT_Chunk sdat_chunk;
				memcpy(&sdat_chunk.chunk_id, SDAT_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
				fwrite(&sdat_chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
				sdat_chunk.chunkSize = talkies[i]->size + sizeof(SDAT_Chunk) - sizeof(sdat_chunk.data);
				memcpy(&chunk_size, &sdat_chunk.chunkSize, sizeof(uint32_t));
				utils::reverseBytes(chunk_size, sizeof(uint32_t));
				fwrite(chunk_size, sizeof(uint32_t), 1, file);
				fwrite(talkies[i]->data, talkies[i]->size, 1, file);
			}

			fclose(file);

			System::Windows::Forms::MessageBox::Show("Successfully compiled file.", "Compilation successful", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);

			return 0;
        }

		/*
			* So there is a special order here that matters for the SONG file.
			* It opens with a SONG chunk.
			* Then it follows with a SGHD chunk.
			* Then follows a whole list of sgens.
			* After that, there will be series of DIGI/HSHD/SDAT chunks in that order.
			* This makes it complicated to determine the exact size and positions without doing a bunch of calculations first.
		*/
        int compileHE4(std::string path)
        {
			FILE* file = nullptr;

			// Open the file.
			fopen_s(&file, path.c_str(), "wb");
			if (file == nullptr)
			{
				System::Windows::Forms::MessageBox::Show("Cannot write to file.", "Compilation failed", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
				return 1;
			}

			// Go through all entries, see if they are song entries and if they have sgens.
			std::vector<SongEntry*> songs;
			for (size_t i = 0; i < entryContainer.size(); i++)
			{
				if (entryContainer[i]->fileType == EntryType::EntryType_Song)
				{
					SongEntry* song = static_cast<SongEntry*>(entryContainer[i]);
					if (song->hasSGEN)
						songs.push_back(song);
				}
			}

			// Make a vector of chunks.
			std::vector<uaudio::wave_reader::ChunkHeader*> chunks;

			// Add root chunk.
			SONG_Chunk song_chunk;
			memcpy(song_chunk.chunk_id, SONG_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
			// The size of the song chunk will be changed later on when all the other chunks are constructed.
			song_chunk.chunkSize = sizeof(SONG_Chunk);

			// Add sghd chunk and set the number of songs.
			SGHD_Chunk sghd_chunk;
			memcpy(sghd_chunk.chunk_id, SGHD_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
			sghd_chunk.chunkSize = sizeof(SGHD_Chunk);
			sghd_chunk.num_of_songs = songs.size();
			ZeroMemory(sghd_chunk.unknown1, sizeof(sghd_chunk.unknown1));

			// Add to the chunk size of the root chunk.
			song_chunk.chunkSize += sghd_chunk.chunkSize;
			
			// Go through all songs and add the sgen chunks first.
			size_t total_data_size = 0;
			for (size_t i = 0; i < songs.size(); i++)
			{
				SGEN_Chunk* sgen_chunk = new SGEN_Chunk();
				memcpy(&sgen_chunk->chunk_id, SGEN_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
				sgen_chunk->chunkSize = sizeof(SGEN_Chunk);
				sgen_chunk->id = 8000 + i + 1;
				sgen_chunk->song_size =
					songs[i]->size +
					sizeof(DIGI_Chunk) +
					sizeof(HSHD_Chunk) +
					sizeof(SDAT_Chunk) - sizeof(unsigned char*);

				song_chunk.chunkSize += sgen_chunk->chunkSize;

				sgen_chunk->song_pos =
					// Song Chunk.
					sizeof(SONG_Chunk) +
					// SGHD Chunk.
					sizeof(SGHD_Chunk) +
					// All SGEN Chunks.
					(songs.size() * sizeof(SGEN_Chunk)) +
					// Size of all the DIGI/SDHD/SDAT chunks.
					total_data_size;

				// Add the data size to the total size.
				total_data_size += sgen_chunk->song_size;

				chunks.push_back(sgen_chunk);
			}

			// Add the final size of all the DIGI/SDHD/SDAT chunks.
			song_chunk.chunkSize += total_data_size;

			unsigned char chunk_size[sizeof(uint32_t)];

			// Write the song chunk first (We cannot put it in the vector because the size is the entire file and it would skip the rest).
			fwrite(&song_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			memcpy(&chunk_size, &song_chunk.chunkSize, sizeof(uint32_t));
			utils::reverseBytes(chunk_size, sizeof(uint32_t));
			fwrite(chunk_size, sizeof(uint32_t), 1, file);

			// Write the song chunk first (We cannot put it in the vector because the size is the entire file and it would skip the rest).
			fwrite(&sghd_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			memcpy(&chunk_size, &sghd_chunk.chunkSize, sizeof(uint32_t));
			utils::reverseBytes(chunk_size, sizeof(uint32_t));
			fwrite(chunk_size, sizeof(uint32_t), 1, file);
			fwrite(utils::add(&sghd_chunk, sizeof(uaudio::wave_reader::ChunkHeader)), sghd_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1, file);

			// Go through every chunk (sghd and sgen).
			for (size_t i = 0; i < chunks.size(); i++)
			{
				fwrite(chunks[i], uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
				memcpy(&chunk_size, &chunks[i]->chunkSize, sizeof(uint32_t));
				utils::reverseBytes(chunk_size, sizeof(uint32_t));
				fwrite(chunk_size, sizeof(uint32_t), 1, file);
				fwrite(utils::add(chunks[i], sizeof(uaudio::wave_reader::ChunkHeader)), chunks[i]->chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1, file);
			}

			for (size_t i = 0; i < songs.size(); i++)
			{
				DIGI_Chunk digi_chunk;
				memcpy(&digi_chunk.chunk_id, DIGI_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
				fwrite(&digi_chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
				digi_chunk.chunkSize = 
					sizeof(DIGI_Chunk) +
					sizeof(HSHD_Chunk) +
					(sizeof(SDAT_Chunk) - sizeof(unsigned char*)) + 
					songs[i]->size;
				memcpy(&chunk_size, &digi_chunk.chunkSize, sizeof(uint32_t));
				utils::reverseBytes(chunk_size, sizeof(uint32_t));
				fwrite(chunk_size, sizeof(uint32_t), 1, file);

				HSHD_Chunk hshd_chunk;
				memcpy(&hshd_chunk.chunk_id, HSHD_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
				fwrite(&hshd_chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
				hshd_chunk.chunkSize = sizeof(HSHD_Chunk);
				hshd_chunk.sample_rate = songs[i]->sample_rate;
				memcpy(&chunk_size, &hshd_chunk.chunkSize, sizeof(uint32_t));
				utils::reverseBytes(chunk_size, sizeof(uint32_t));
				fwrite(chunk_size, sizeof(uint32_t), 1, file);
				fwrite(utils::add(&hshd_chunk, sizeof(uaudio::wave_reader::ChunkHeader)), hshd_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1, file);

				SDAT_Chunk sdat_chunk;
				memcpy(&sdat_chunk.chunk_id, SDAT_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
				fwrite(&sdat_chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
				sdat_chunk.chunkSize = songs[i]->size + sizeof(SDAT_Chunk) - sizeof(sdat_chunk.data);
				memcpy(&chunk_size, &sdat_chunk.chunkSize, sizeof(uint32_t));
				utils::reverseBytes(chunk_size, sizeof(uint32_t));
				fwrite(chunk_size, sizeof(uint32_t), 1, file);
				fwrite(songs[i]->data, songs[i]->size, 1, file);
			}

			fclose(file);

			System::Windows::Forms::MessageBox::Show("Successfully compiled file.", "Compilation successful", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);

			return 0;
        }

		int compile(std::string path)
		{
			switch (entryContainer.fileType)
			{
				case FileType_A:
				{
					break;
				}
				case FileType_HE2:
				{
					return compileHE2(path);
					break;
				}
				case FileType_HE4:
				{
					return compileHE4(path);
					break;
				}
				default:
					break;
			}
			return 1;
		}
    }
}