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
		uaudio::wave_reader::ChunkHeader lastChunk = uaudio::wave_reader::ChunkHeader();
		std::string extraData;

		int getChunk(uaudio::wave_reader::ChunkHeader& chunk, CFILE& file)
		{
			// Store the chunk size so that it can be reversed (from little to big endian).
			unsigned char chunk_size[sizeof(uint32_t)];
			// Read the chunk id first.
			file.cfread(&chunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
			file.cfread(&chunk_size, sizeof(uint32_t), 1);
			chunk.chunkSize = utils::little_to_big_endian<uint32_t>(chunk_size);
			memcpy(&lastChunk, &chunk, sizeof(uaudio::wave_reader::ChunkHeader));
			if (chunk.chunkSize > file.size)
				return err_chunk_size_exceeds_file_size;
			return err_ok;
		}

		int getSGENChunk(SGEN_Chunk& chunk, CFILE& file)
		{
			int err = getChunk(chunk, file);
			if (err != err_ok)
				return err;
			// Read the rest of the sgen data.
			file.cfread(utils::add(&chunk, sizeof(uaudio::wave_reader::ChunkHeader)), chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1);
			return err_ok;
		}

		int getDIGIChunk(DIGI_Chunk& chunk, CFILE& file)
		{
			int err = getChunk(chunk, file);
			if (err != err_ok)
				return err;
			return err_ok;
		}

		int getSGHDChunk(SGHD_Chunk& chunk, CFILE& file)
		{
			int err = getChunk(chunk, file);
			if (err != err_ok)
				return err;
			// Read the rest of the sghd data.
			file.cfread(utils::add(&chunk, sizeof(uaudio::wave_reader::ChunkHeader)), chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1);
			if (((sizeof(SDAT_Chunk) - sizeof(unsigned char*) + sizeof(DIGI_Chunk) + sizeof(HSHD_Chunk) + sizeof(SGEN_Chunk)) * chunk.num_of_songs > file.size))
				return err_num_songs_exceeds_file_size;
			return err_ok;
		}

		int getHSHDChunk(HSHD_Chunk& chunk, CFILE& file)
		{
			int err = getChunk(chunk, file);
			if (err != err_ok)
				return err;
			// Read the rest of the hshd data.
			file.cfread(
				utils::add(&chunk, sizeof(uaudio::wave_reader::ChunkHeader)),
				chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader),
				1
			);
			return err_ok;
		}

		int getSDATChunk(SDAT_Chunk& chunk, CFILE& file)
		{
			int err = getChunk(chunk, file);
			if (err != err_ok)
				return err;
			// Read the rest of the sdat data.
			chunk.data = reinterpret_cast<unsigned char*>(malloc(chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader)));
			file.cfread(
				chunk.data,
				chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader),
				1
			);
			return err_ok;
		}

		int getTALKChunk(TALK_Chunk& chunk, CFILE& file)
		{
			int err = getChunk(chunk, file);
			if (err != err_ok)
				return err;
			return err_ok;
		}

		int ReadSongEntry(SongEntry* songEntry, CFILE& file, size_t& size, bool sgen)
		{
			uint32_t pos = file.cftell();

			// Get the digi, hsdh and sdat chunks.
			DIGI_Chunk digi = DIGI_Chunk();
			int err = getDIGIChunk(digi, file);
			if (err != err_ok)
				return err;
			if (utils::chunkcmp(digi.chunk_id, DIGI_CHUNK_ID) != 0)
			{
				extraData = std::string(DIGI_CHUNK_ID);
				return err_chunk_name_missmatch;
			}

			size += sizeof(digi);

			HSHD_Chunk hshd = HSHD_Chunk();
			err = getHSHDChunk(hshd, file);
			if (err != err_ok)
				return err;
			if (utils::chunkcmp(hshd.chunk_id, HSHD_CHUNK_ID) != 0)
			{
				extraData = std::string(HSHD_CHUNK_ID);
				return err_chunk_name_missmatch;
			}

			size += sizeof(hshd);

			SDAT_Chunk sdat = SDAT_Chunk();
			err = getSDATChunk(sdat, file);
			if (err != err_ok)
				return err;
			if (utils::chunkcmp(sdat.chunk_id, SDAT_CHUNK_ID) != 0)
			{
				extraData = std::string(SDAT_CHUNK_ID);
				return err_chunk_name_missmatch;
			}

			size += sdat.chunkSize;

			songEntry->size = sdat.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader);
			songEntry->pos = pos;
			songEntry->sample_rate = hshd.sample_rate;
			songEntry->hasSGEN = sgen;
			songEntry->data = reinterpret_cast<unsigned char*>(malloc(songEntry->size));
			if (songEntry->data != nullptr)
				memcpy(songEntry->data, sdat.data, songEntry->size);

			return err_ok;
		}

		int throw_error(int error_code, CFILE& file)
		{
			switch (error_code)
			{
				case err_ok:
				{
					System::String^ text = "Successfully decompiled resource file (" + entryContainer.size().ToString() + " resources)";
					MessageBox::Show(text, "Decompilation successful", MessageBoxButtons::OK, MessageBoxIcon::Information);
					break;
				}
				case err_file_empty:
				{
					MessageBox::Show("File is empty.", "Decompilation failed", MessageBoxButtons::OK, MessageBoxIcon::Information);
					break;
				}
				case err_chunk_size_exceeds_file_size:
				{
					System::String^ text =
						"Chunk named \"" +
						gcnew System::String(reinterpret_cast<char*>(lastChunk.chunk_id)) +
						"\" has a chunk size that exceeds the total file size (" +
						gcnew System::String(std::to_string(lastChunk.chunkSize).c_str()) +
						">" + 
						gcnew System::String(std::to_string(file.size).c_str()) +
						").";
					MessageBox::Show(text, "Decompilation failed", MessageBoxButtons::OK, MessageBoxIcon::Information);
					break;
				}
				case err_num_songs_exceeds_file_size:
				{
					MessageBox::Show("Number of songs exceeds possible file size.", "Decompilation failed", MessageBoxButtons::OK, MessageBoxIcon::Information);
					break;
				}
				case err_chunk_unrecognized:
				{
					System::String^ text =
						"Chunk named \"" +
						gcnew System::String(reinterpret_cast<char*>(lastChunk.chunk_id)) +
						"\" is not recognized.";
					MessageBox::Show(text, "Decompilation failed", MessageBoxButtons::OK, MessageBoxIcon::Information);
					break;
				}
				case err_bad_file_start:
				{
					System::String^ text =
						"Bad File Start: \"" +
						gcnew System::String(reinterpret_cast<char*>(lastChunk.chunk_id)) +
						"\"";
					MessageBox::Show(text, "Decompilation failed", MessageBoxButtons::OK, MessageBoxIcon::Information);
					break;
				}
				case err_chunk_name_missmatch:
				{
					System::String^ text =
						"Chunk missmatch: \"" +
						gcnew System::String(reinterpret_cast<char*>(lastChunk.chunk_id)) +
						"\" \"" +
						gcnew System::String(extraData.c_str()) + 
						"\"";
					MessageBox::Show(text, "Decompilation failed", MessageBoxButtons::OK, MessageBoxIcon::Information);
					break;
				}
			}
			return error_code;
		}

		int decompile()
		{
			int err = err_ok;

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];

			// Create a file (my own wrapper because sometimes data is encrypted).
			CFILE cfile;
			cfile.cfopen_s(entryContainer.filePath.c_str(), "rb");

			if (cfile.size == 0)
			{
				err = err_file_empty;
				return throw_error(err, cfile);
			}

			// Get the first chunk.
			uaudio::wave_reader::ChunkHeader rootChunk;

			cfile.cfread(&rootChunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
			memcpy(&lastChunk.chunk_id, &rootChunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE);
			cfile.crewind();

			// If the chunk is unknown, it might be encryped so we will decrypt.
			if (utils::chunkcmp(rootChunk.chunk_id, TLKB_CHUNK_ID) != 0 && utils::chunkcmp(rootChunk.chunk_id, SONG_CHUNK_ID) != 0)
			{
				// Only encrypt first 8 bytes and then compare (instead of whole file).
				utils::xorShift(cfile.start, uaudio::wave_reader::CHUNK_ID_SIZE, 0x69);
				cfile.cfread(&rootChunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
				memcpy(&lastChunk.chunk_id, &rootChunk.chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE);
				// TODO: DEBUG.
				//FILE* saveFile = nullptr;
				//fopen_s(&saveFile, "D:/ekkes/test.(a)", "wb");

				//fwrite(cfile.start, cfile.size, 1, saveFile);
				//fclose(saveFile);

				// If the first chunk is still not recognized, just throw an error.
				if (utils::chunkcmp(rootChunk.chunk_id, LECF_CHUNK_ID) != 0)
				{
					err = err_bad_file_start;
					return throw_error(err, cfile);
				}

				cfile.crewind();

				// It was indeed an encrypted file. Decrypt the rest.
				utils::xorShift(cfile.start, uaudio::wave_reader::CHUNK_ID_SIZE, 0x69);
				utils::xorShift(cfile.start, cfile.size, 0x69);
			}

			err = getChunk(rootChunk, cfile);
			if (err != err_ok)
				return throw_error(err, cfile);

			size_t read_bytes = sizeof(uaudio::wave_reader::ChunkHeader);
			// As long as the file is not eof it will go on.
			while (read_bytes < cfile.size)
			{
				// Get first chunk.
				uaudio::wave_reader::ChunkHeader chunk;
				err = getChunk(chunk, cfile);
				if (err != err_ok)
					return throw_error(err, cfile);

				// Revert to before the chunk was set.
				cfile.cfseek(-sizeof(chunk), SEEK_CUR);

				// Compare it to known chunk ids.
				if (utils::chunkcmp(chunk.chunk_id, SGHD_CHUNK_ID) == 0)
				{
					// Basically ignore this one.
					SGHD_Chunk sghd;
					err = getSGHDChunk(sghd, cfile);
					if (err != err_ok)
						return throw_error(err, cfile);

					read_bytes += sizeof(sghd);
				}
				else if (utils::chunkcmp(chunk.chunk_id, SGEN_CHUNK_ID) == 0)
				{
					// Basically ignore this one.
					SGEN_Chunk sgen;
					err = getSGENChunk(sgen, cfile);
					if (err != err_ok)
						return throw_error(err, cfile);

					read_bytes += sizeof(sgen);

					size_t after_sgen_pos = cfile.cftell();
					cfile.cfseek(sgen.song_pos, SEEK_SET);

					SongEntry* songEntry = new SongEntry();
					err = ReadSongEntry(songEntry, cfile, read_bytes, true);
					if (err != err_ok)
						return throw_error(err, cfile);

					cfile.cfseek(after_sgen_pos, SEEK_SET);

					entryContainer.AddEntry(songEntry);
				}
				// DIGI is always a song, so we'll hardcode other chunks.
				else if (utils::chunkcmp(chunk.chunk_id, DIGI_CHUNK_ID) == 0)
				{
					SongEntry* songEntry = new SongEntry();
					ReadSongEntry(songEntry, cfile, read_bytes);
					if (err != err_ok)
						return throw_error(err, cfile);

					entryContainer.AddEntry(songEntry);
				}
				// Talk is always a talkie file (voice usually, but can also be sfx).
				else if (utils::chunkcmp(chunk.chunk_id, TALK_CHUNK_ID) == 0)
				{
					uint32_t pos = cfile.cftell();
					// Get the talk and hshd chunks.
					TALK_Chunk talk;
					err = getTALKChunk(talk, cfile);
					if (err != err_ok)
						return throw_error(err, cfile);
					if (utils::chunkcmp(talk.chunk_id, TALK_CHUNK_ID) != 0)
					{
						extraData = std::string(TALK_CHUNK_ID);
						return throw_error(err_chunk_name_missmatch, cfile);
					}

					read_bytes += sizeof(talk);

					HSHD_Chunk hshd;
					err = getHSHDChunk(hshd, cfile);
					if (err != err_ok)
						return throw_error(err, cfile);
					if (utils::chunkcmp(hshd.chunk_id, HSHD_CHUNK_ID) != 0)
					{
						extraData = std::string(HSHD_CHUNK_ID);
						return throw_error(err_chunk_name_missmatch, cfile);
					}

					read_bytes += sizeof(hshd);

					// Set whether it has a sbng chunk to false by default.
					size_t sbng_size = 0;

					err = getChunk(chunk, cfile);
					if (err != err_ok)
						return throw_error(err, cfile);

					// There can be a sbng chunk before it.
					if (utils::chunkcmp(chunk.chunk_id, SBNG_CHUNK_ID) == 0)
					{
						sbng_size = chunk.chunkSize;

						// sbng chunk has been found and will be skipped.
						cfile.cfseek(sbng_size - sizeof(uaudio::wave_reader::ChunkHeader), SEEK_CUR);
						read_bytes += sbng_size;

						sbng_size = sbng_size - sizeof(uaudio::wave_reader::ChunkHeader);
					}
					// Else, revert to before the chunk was loaded.
					else
						cfile.cfseek(-sizeof(uaudio::wave_reader::ChunkHeader), SEEK_CUR);

					SDAT_Chunk sdat;
					err = getSDATChunk(sdat, cfile);
					if (err != err_ok)
						return throw_error(err, cfile);
					if (utils::chunkcmp(sdat.chunk_id, SDAT_CHUNK_ID) != 0)
					{
						extraData = std::string(SDAT_CHUNK_ID);
						return throw_error(err_chunk_name_missmatch, cfile);
					}

					read_bytes += sizeof(sdat) - sizeof(sdat.data);
					read_bytes += sdat.chunkSize;

					TalkieEntry* talkieEntry = new TalkieEntry();
					talkieEntry->pos = pos;
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
				node->Text = node->Name;
				if (notes::noteContainer.GetNote(node->num) != nullptr)
					node->Text += gcnew System::String(" (") + notes::noteContainer.GetNote(node->num) + gcnew System::String(")");
				form->entryView->Nodes->Add(node);
			}
			return throw_error(err, cfile);
		}
	}
}
