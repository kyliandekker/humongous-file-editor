#include "Humongous/Decompiler.h"

#include <cstdio>
#include <iostream>
#include <uaudio_wave_reader/ChunkHeader.h>

#include "utils/utils.h"
#include "Humongous/HumongousChunks.h"
#include "Humongous/File.h"
#include "Humongous/FileContainer.h"
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

			fileContainer.Clear();
			form->fileView->Nodes->Clear();
			for (size_t i = form->tabControl1->TabPages->Count - 1; i > 0; i--)
				form->tabControl1->TabPages->RemoveAt(i);

			CFILE cfile;
			cfile.cfopen_s(path.c_str(), "rb");

			uaudio::wave_reader::ChunkHeader rootChunk;
			getChunk(rootChunk, cfile);

			if (!utils::chunkcmp(reinterpret_cast<char*>(rootChunk.chunk_id), TLKB_CHUNK_ID) && !utils::chunkcmp(reinterpret_cast<char*>(rootChunk.chunk_id), SONG_CHUNK_ID))
			{
				cfile.crewind();
				cfile.start = utils::xorShift(cfile.start, cfile.size, 0x69);
				getChunk(rootChunk, cfile);
			}

			while (cfile.cftell() < rootChunk.chunkSize)
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
					SongFile* songFile = new SongFile();
					songFile->num = fileContainer.files.size();
					songFile->size = sdat.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader);
					songFile->sample_rate = hshd.sample_rate;
					songFile->data = reinterpret_cast<unsigned char*>(malloc(songFile->size));
					memcpy(songFile->data, sdat.data, songFile->size);

					std::string num = std::to_string(songFile->num) + ".wav";
					fileContainer.files.push_back(songFile);
					System::String^ text = gcnew System::String(num.c_str());
					HumongousNode^ node;
					node = (gcnew HumongousNode);
					node->Text = text;
					node->Name = text;
					node->num = fileContainer.files.size() - 1;
					form->fileView->Nodes->Add(node);
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

					TalkieFile* talkieFile = new TalkieFile();
					talkieFile->num = fileContainer.files.size();
					talkieFile->size = sdat.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader);
					talkieFile->sample_rate = hshd.sample_rate;
					talkieFile->data = reinterpret_cast<unsigned char*>(malloc(talkieFile->size));
					talkieFile->hasSbng = hasSbng;
					memcpy(talkieFile->data, sdat.data, talkieFile->size);

					std::string num = std::to_string(talkieFile->num) + ".wav";
					fileContainer.files.push_back(talkieFile);
					System::String^ text = gcnew System::String(num.c_str());
					HumongousNode^ node;
					node = (gcnew HumongousNode);
					node->Text = text;
					node->Name = text;
					node->num = fileContainer.files.size() - 1;
					form->fileView->Nodes->Add(node);
				}
				else
				{
					cfile.cfseek(chunk.chunkSize, SEEK_CUR);
				}

				int tell = cfile.cftell();
				float progress = 100.0f / rootChunk.chunkSize * tell;
				form->toolProgressBar->Value = progress;
				if (progress > 0)
					form->toolProgressBar->Value = progress - 1;
				if (progress == 100.0f)
					form->toolProgressBar->Value = progress;
			}

			System::String^ text = "Successfully decompiled resource file (" + fileContainer.files.size().ToString() + " resources)";
			MessageBox::Show(text, "Decompilation successful", MessageBoxButtons::OK, MessageBoxIcon::Information);
			return 0;
		}
	}
}
