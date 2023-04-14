#include "Humongous/Decompiler.h"

#include <cstdio>
#include <iostream>
#include <uaudio_wave_reader/ChunkHeader.h>

#include "utils/utils.h"
#include "Humongous/HumongousChunks.h"
#include "Humongous/Entry.h"
#include "Humongous/EntryContainer.h"
#include "HumongousEditorForm.h"
#include "CFILE.h"

namespace HumongousFileEditor
{
	namespace decompiler
	{
		void getChunk(uaudio::wave_reader::ChunkHeader& chunk, CFILE& file)
		{
			// Store the chunk size so that it can be reversed (from little to big endian).
			unsigned char chunk_size[sizeof(uint32_t)];
			// Read the chunk id first.
			file.cfread(&chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
			file.cfread(&chunk_size, sizeof(uint32_t), 1);
			chunk.chunkSize = utils::little_to_big_endian<uint32_t>(chunk_size);
		}

		SGEN_Chunk getSGENChunk(CFILE& file)
		{
			SGEN_Chunk sgen_chunk = SGEN_Chunk();
			getChunk(sgen_chunk, file);
			// Read the rest of the sgen data.
			file.cfread(utils::add(&sgen_chunk, sizeof(uaudio::wave_reader::ChunkHeader)), sgen_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1);
			return sgen_chunk;
		}

		DIGI_Chunk getDIGIChunk(CFILE& file)
		{
			DIGI_Chunk digi_chunk = DIGI_Chunk();
			getChunk(digi_chunk, file);
			return digi_chunk;
		}

		SGHD_Chunk getSGHDChunk(CFILE& file)
		{
			SGHD_Chunk sghd_chunk = SGHD_Chunk();
			getChunk(sghd_chunk, file);
			// Read the rest of the sghd data.
			file.cfread(utils::add(&sghd_chunk, sizeof(uaudio::wave_reader::ChunkHeader)), sghd_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1);
			return sghd_chunk;
		}

		HSHD_Chunk getHSHDChunk(CFILE& file)
		{
			HSHD_Chunk hshd_chunk = HSHD_Chunk();
			getChunk(hshd_chunk, file);
			// Read the rest of the hshd data.
			file.cfread(
				utils::add(&hshd_chunk, sizeof(uaudio::wave_reader::ChunkHeader)),
				hshd_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader),
				1
			);
			return hshd_chunk;
		}

		SDAT_Chunk getSDATChunk(CFILE& file)
		{
			SDAT_Chunk sdat_chunk = SDAT_Chunk();
			getChunk(sdat_chunk, file);
			// Read the rest of the sdat data.
			sdat_chunk.data = reinterpret_cast<unsigned char*>(malloc(sdat_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader)));
			file.cfread(
				sdat_chunk.data,
				sdat_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader),
				1
			);
			return sdat_chunk;
		}

        TALK_Chunk getTALKChunk(CFILE& file)
        {
			TALK_Chunk talk_chunk = TALK_Chunk();
			getChunk(talk_chunk, file);
			return talk_chunk;
        }

		Entry* ReadSongEntry(CFILE& file, size_t& size, bool sgen)
		{
			// Get the digi, hsdh and sdat chunks.
			DIGI_Chunk digi = getDIGIChunk(file);
			size += sizeof(digi);
			HSHD_Chunk hshd = getHSHDChunk(file);
			size += sizeof(hshd);
			SDAT_Chunk sdat = getSDATChunk(file);
			size += sdat.chunkSize;

			int num = entryContainer.size();

			// Create an entry with the loaded data.
			SongEntry* songEntry = new SongEntry();
			songEntry->size = sdat.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader);
			songEntry->sample_rate = hshd.sample_rate;
			songEntry->data = reinterpret_cast<unsigned char*>(malloc(songEntry->size));
			songEntry->hasSGEN = sgen;
			memcpy(songEntry->data, sdat.data, songEntry->size);
			return songEntry;
		}

		int decompile()
		{
			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];

			// Create a file (my own wrapper because sometimes data is encrypted).
			CFILE cfile;
			cfile.cfopen_s(entryContainer.filePath.c_str(), "rb");

			// Get the first chunk.
			uaudio::wave_reader::ChunkHeader rootChunk;
			getChunk(rootChunk, cfile);

			// If the chunk is unknown, it might be encryped so we will unencrypt.
			if (utils::chunkcmp(rootChunk.chunk_id, TLKB_CHUNK_ID) != 0 && utils::chunkcmp(rootChunk.chunk_id, SONG_CHUNK_ID) != 0)
			{
				cfile.crewind();
				cfile.start = utils::xorShift(cfile.start, cfile.size, 0x69);
				getChunk(rootChunk, cfile);

				// TODO: DEBUG.
				//FILE* saveFile = nullptr;
				//fopen_s(&saveFile, "D:/ekkes/test.(a)", "wb");

				//fwrite(cfile.start, cfile.size, 1, saveFile);
				//fclose(saveFile);

				// If the first chunk is still not recognized, just throw an error.
				if (utils::chunkcmp(rootChunk.chunk_id, LECF_CHUNK_ID) != 0)
				{
					System::Windows::Forms::MessageBox::Show("File cannot be opened because it is not recognized as a Humongous file.", "Cannot open file", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
					return 1;
				}
			}

			size_t read_bytes = sizeof(uaudio::wave_reader::ChunkHeader);
			// As long as the file is not eof it will go on.
			while (read_bytes < cfile.size)
			{
				// Get first chunk.
				uaudio::wave_reader::ChunkHeader chunk;
				getChunk(chunk, cfile);

				// Revert to before the chunk was set.
				cfile.cfseek(-sizeof(uaudio::wave_reader::ChunkHeader), SEEK_CUR);

				// Compare it to known chunk ids.
				if (utils::chunkcmp(chunk.chunk_id, SGHD_CHUNK_ID) == 0)
				{
					// Basically ignore this one.
					SGHD_Chunk sghd = getSGHDChunk(cfile);
					read_bytes += sizeof(sghd);
				}
				else if (utils::chunkcmp(chunk.chunk_id, SGEN_CHUNK_ID) == 0)
				{
					// Basically ignore this one.
					SGEN_Chunk sgen = getSGENChunk(cfile);
					read_bytes += sizeof(sgen);

					size_t after_sgen_pos = cfile.cftell();
					cfile.cfseek(sgen.song_pos, SEEK_SET);
					Entry* songEntry = ReadSongEntry(cfile, read_bytes, true);
					cfile.cfseek(after_sgen_pos, SEEK_SET);

					entryContainer.AddEntry(songEntry);
				}
				// DIGI is always a song, so we'll hardcode other chunks.
				else if (utils::chunkcmp(chunk.chunk_id, DIGI_CHUNK_ID) == 0)
				{
					Entry* songEntry = ReadSongEntry(cfile, read_bytes);
					entryContainer.AddEntry(songEntry);
				}
				// Talk is always a talkie file (voice usually, but can also be sfx).
				else if (utils::chunkcmp(chunk.chunk_id, TALK_CHUNK_ID) == 0)
				{
					// Get the talk and hshd chunks.
					TALK_Chunk talk = getTALKChunk(cfile);
					read_bytes += sizeof(talk);
					HSHD_Chunk hshd = getHSHDChunk(cfile);
					read_bytes += sizeof(hshd);

					// Set whether it has a sbng chunk to false by default.
					size_t sbng_size = 0;

					// There can be a sbng chunk before it.
					unsigned char chunk[uaudio::wave_reader::CHUNK_ID_SIZE];
					cfile.cfread(chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
					if (utils::chunkcmp(chunk, SBNG_CHUNK_ID) == 0)
					{
						// sbng chunk has been found and will be skipped.
						unsigned char chunk_size[sizeof(uint32_t)];
						cfile.cfread(&chunk_size, sizeof(uint32_t), 1);
						uint32_t sbng_size = utils::little_to_big_endian<uint32_t>(chunk_size);
						cfile.cfseek(sbng_size - sizeof(uaudio::wave_reader::ChunkHeader), SEEK_CUR);
						read_bytes += sbng_size;

						sbng_size = sbng_size - sizeof(uaudio::wave_reader::ChunkHeader);
					}
					// Else, revert to before the chunk was loaded.
					else
						cfile.cfseek(-uaudio::wave_reader::CHUNK_ID_SIZE, SEEK_CUR);

					SDAT_Chunk sdat = getSDATChunk(cfile);
					read_bytes += sizeof(sdat) - sizeof(sdat.data);
					read_bytes += sdat.chunkSize;

					TalkieEntry* talkieEntry = new TalkieEntry();
					talkieEntry->size = sdat.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader);
					talkieEntry->sample_rate = hshd.sample_rate;
					talkieEntry->data = reinterpret_cast<unsigned char*>(malloc(talkieEntry->size));
					talkieEntry->sbng_size = sbng_size;
					memcpy(talkieEntry->data, sdat.data, talkieEntry->size);
					entryContainer.AddEntry(talkieEntry);
				}
				else
				{
					cfile.cfseek(chunk.chunkSize, SEEK_CUR);
					read_bytes += chunk.chunkSize;
				}

				int tell = cfile.cftell();
				float progress = 100.0f / cfile.size * tell;
				form->toolProgressBar->Value = progress;
				if (progress > 0)
					form->toolProgressBar->Value = progress - 1;
				if (progress == 100.0f)
					form->toolProgressBar->Value = progress;
			}

			System::String^ text = "Successfully decompiled resource file (" + entryContainer.size().ToString() + " resources)";
			MessageBox::Show(text, "Decompilation successful", MessageBoxButtons::OK, MessageBoxIcon::Information);

			notes::noteContainer.LoadNotes(entryContainer.filePath);

			// Add all loaded entries to the node container.
			for (size_t i = 0; i < entryContainer.size(); i++)
			{
				std::string num = std::to_string(entryContainer[i]->num) + entryContainer[i]->GetCommonExtension();
				System::String^ text = gcnew System::String(num.c_str());
				HumongousNode^ node;

				node = (gcnew HumongousNode);
				node->Name = text;
				node->num = i;
				node->Text = node->Name + gcnew System::String(" (") + notes::noteContainer.GetNote(node->num) + gcnew System::String(")");
				form->entryView->Nodes->Add(node);
			}
			return 0;
		}
	}
}
