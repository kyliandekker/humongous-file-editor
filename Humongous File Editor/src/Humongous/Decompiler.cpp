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
			unsigned char chunk_size[sizeof(uint32_t)];
			file.cfread(&chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
			file.cfread(&chunk_size, sizeof(uint32_t), 1);
			chunk.chunkSize = utils::big_to_little_endian(chunk_size);
		}

		SGEN_Chunk getSGENChunk(CFILE& file)
		{
			SGEN_Chunk sgen_chunk = SGEN_Chunk();
			unsigned char chunk_size[sizeof(uint32_t)];
			file.cfread(&sgen_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
			file.cfread(&chunk_size, sizeof(uint32_t), 1);
			sgen_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
			file.cfread(uaudio::utils::add(&sgen_chunk, sizeof(uaudio::wave_reader::ChunkHeader)), sgen_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1);
			return sgen_chunk;
		}

		DIGI_Chunk getDIGIChunk(CFILE& file)
		{
			DIGI_Chunk digi_chunk = DIGI_Chunk();
			unsigned char chunk_size[sizeof(uint32_t)];
			file.cfread(&digi_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
			file.cfread(&chunk_size, sizeof(uint32_t), 1);
			digi_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
			return digi_chunk;
		}

		SGHD_Chunk getSGHDChunk(CFILE& file)
		{
			SGHD_Chunk sghd_chunk = SGHD_Chunk();
			unsigned char chunk_size[sizeof(uint32_t)];
			file.cfread(&sghd_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
			file.cfread(&chunk_size, sizeof(uint32_t), 1);
			sghd_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
			file.cfread(uaudio::utils::add(&sghd_chunk, sizeof(uaudio::wave_reader::ChunkHeader)), sghd_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1);
			return sghd_chunk;
		}


		/// <summary>
		/// Returns a hshd chunk.
		/// </summary>
		/// <param name="file">The file pointer.</param>
		/// <returns></returns>
		HSHD_Chunk getHSHDChunk(CFILE& file)
		{
			HSHD_Chunk hshd_chunk = HSHD_Chunk();
			unsigned char chunk_size[sizeof(uint32_t)];
			file.cfread(&hshd_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
			file.cfread(&chunk_size, sizeof(uint32_t), 1);
			hshd_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
			file.cfread(
				uaudio::utils::add(&hshd_chunk, sizeof(uaudio::wave_reader::ChunkHeader)),
				hshd_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader),
				1
			);
			return hshd_chunk;
		}

		/// <summary>
		/// Returns a sdat chunk.
		/// </summary>
		/// <param name="file">The file pointer.</param>
		/// <returns></returns>
		SDAT_Chunk getSDATChunk(CFILE& file)
		{
			SDAT_Chunk sdat_chunk = SDAT_Chunk();
			unsigned char chunk_size[sizeof(uint32_t)];
			file.cfread(&sdat_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
			file.cfread(&chunk_size, sizeof(uint32_t), 1);
			sdat_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
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
			unsigned char chunk_size[sizeof(uint32_t)];
			file.cfread(&talk_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
			file.cfread(&chunk_size, sizeof(uint32_t), 1);
			talk_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
			return talk_chunk;
        }

		int decompile(std::string path)
		{
			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];

			CFILE cfile;
			cfile.cfopen_s(path.c_str(), "rb");

			uaudio::wave_reader::ChunkHeader rootChunk;
			getChunk(rootChunk, cfile);

			if (!utils::chunkcmp(reinterpret_cast<char*>(rootChunk.chunk_id), TLKB_CHUNK_ID) && !utils::chunkcmp(reinterpret_cast<char*>(rootChunk.chunk_id), SONG_CHUNK_ID))
			{
				cfile.crewind();
				cfile.start = utils::xorShift(cfile.start, cfile.size, 0x69);
				getChunk(rootChunk, cfile);

				FILE* saveFile = nullptr;
				fopen_s(&saveFile, "D:/ekkes/test.(a)", "wb");

				fwrite(cfile.start, cfile.size, 1, saveFile);
				fclose(saveFile);
			}

			while (cfile.cftell() < cfile.size)
			{
				uaudio::wave_reader::ChunkHeader chunk;
				getChunk(chunk, cfile);

				cfile.cfseek(-sizeof(uaudio::wave_reader::ChunkHeader), SEEK_CUR);

				if (utils::chunkcmp(reinterpret_cast<char*>(chunk.chunk_id), SGHD_CHUNK_ID))
				{
					SGHD_Chunk sghd = getSGHDChunk(cfile);
				}
				else if (utils::chunkcmp(reinterpret_cast<char*>(chunk.chunk_id), SGEN_CHUNK_ID))
				{
					SGEN_Chunk sgen = getSGENChunk(cfile);
				}
				// DIGI is always a song, so we'll hardcode other chunks.
				else if (utils::chunkcmp(reinterpret_cast<char*>(chunk.chunk_id), DIGI_CHUNK_ID))
				{
					DIGI_Chunk digi = getDIGIChunk(cfile);
					HSHD_Chunk hshd = getHSHDChunk(cfile);
					SDAT_Chunk sdat = getSDATChunk(cfile);

					SongEntry* songEntry = new SongEntry();
					songEntry->size = sdat.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader);
					songEntry->sample_rate = hshd.sample_rate;
					songEntry->data = reinterpret_cast<unsigned char*>(malloc(songEntry->size));
					memcpy(songEntry->data, sdat.data, songEntry->size);
					entryContainer.AddEntry(songEntry);
				}
				// Talk is always a talkie file (voice usually, but can also be sfx).
				else if (utils::chunkcmp(reinterpret_cast<char*>(chunk.chunk_id), TALK_CHUNK_ID))
				{
					TALK_Chunk talk = getTALKChunk(cfile);
					HSHD_Chunk hshd = getHSHDChunk(cfile);

					bool hasSbng = false;

					// There can be a sbng chunk before it.
					unsigned char chunk[uaudio::wave_reader::CHUNK_ID_SIZE];
					cfile.cfread(chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1);
					if (utils::chunkcmp(reinterpret_cast<char*>(chunk), SBNG_CHUNK_ID))
					{
						hasSbng = true;

						unsigned char chunk_size[sizeof(uint32_t)];
						cfile.cfread(&chunk_size, sizeof(uint32_t), 1);
						uint32_t sbng_size = utils::big_to_little_endian(chunk_size);
						cfile.cfseek(sbng_size - sizeof(uaudio::wave_reader::ChunkHeader), SEEK_CUR);
					}
					else
						cfile.cfseek(-uaudio::wave_reader::CHUNK_ID_SIZE, SEEK_CUR);

					SDAT_Chunk sdat = getSDATChunk(cfile);

					TalkieEntry* talkieEntry = new TalkieEntry();
					talkieEntry->size = sdat.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader);
					talkieEntry->sample_rate = hshd.sample_rate;
					talkieEntry->data = reinterpret_cast<unsigned char*>(malloc(talkieEntry->size));
					talkieEntry->hasSbng = hasSbng;
					memcpy(talkieEntry->data, sdat.data, talkieEntry->size);
					entryContainer.AddEntry(talkieEntry);
				}
				else
				{
					cfile.cfseek(chunk.chunkSize, SEEK_CUR);
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

			// Add all loaded entries to the node container.
			for (size_t i = 0; i < entryContainer.size(); i++)
			{
				std::string num = std::to_string(entryContainer[i]->num) + entryContainer[i]->GetCommonExtension();
				System::String^ text = gcnew System::String(num.c_str());
				HumongousNode^ node;
				node = (gcnew HumongousNode);
				node->Text = text;
				node->Name = text;
				node->num = entryContainer.size() - 1;
				form->fileView->Nodes->Add(node);
			}
			return 0;
		}
	}
}
