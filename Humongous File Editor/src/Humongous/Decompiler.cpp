#include "Humongous/Decompiler.h"

#include <cstdio>
#include <iostream>
#include <uaudio_wave_reader/ChunkHeader.h>

#include "utils/utils.h"
#include "Humongous/HumongousChunks.h"
#include "Humongous/File.h"
#include "Humongous/FileContainer.h"
#include "HumongousEditorForm.h"

namespace hfe
{
	namespace decompiler
	{
		void getChunk(uaudio::wave_reader::ChunkHeader& chunk, FILE* file)
		{
			unsigned char chunk_size[sizeof(uint32_t)];
			fread(&chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			fread(&chunk_size, sizeof(uint32_t), 1, file);
			chunk.chunkSize = utils::big_to_little_endian(chunk_size);
		}

		SGEN_Chunk getSGENChunk(FILE* file)
		{
			SGEN_Chunk sgen_chunk = SGEN_Chunk();
			unsigned char chunk_size[sizeof(uint32_t)];
			fread(&sgen_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			fread(&chunk_size, sizeof(uint32_t), 1, file);
			sgen_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
			fread(uaudio::utils::add(&sgen_chunk, sizeof(uaudio::wave_reader::ChunkHeader)), sgen_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1, file);
			return sgen_chunk;
		}

		DIGI_Chunk getDIGIChunk(FILE* file)
		{
			DIGI_Chunk digi_chunk = DIGI_Chunk();
			unsigned char chunk_size[sizeof(uint32_t)];
			fread(&digi_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			fread(&chunk_size, sizeof(uint32_t), 1, file);
			digi_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
			return digi_chunk;
		}

		SGHD_Chunk getSGHDChunk(FILE* file)
		{
			SGHD_Chunk sghd_chunk = SGHD_Chunk();
			unsigned char chunk_size[sizeof(uint32_t)];
			fread(&sghd_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			fread(&chunk_size, sizeof(uint32_t), 1, file);
			sghd_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
			fread(uaudio::utils::add(&sghd_chunk, sizeof(uaudio::wave_reader::ChunkHeader)), sghd_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader), 1, file);
			return sghd_chunk;
		}


		/// <summary>
		/// Returns a hshd chunk.
		/// </summary>
		/// <param name="file">The file pointer.</param>
		/// <returns></returns>
		HSHD_Chunk getHSHDChunk(FILE* file)
		{
			HSHD_Chunk hshd_chunk = HSHD_Chunk();
			unsigned char chunk_size[sizeof(uint32_t)];
			fread(&hshd_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			fread(&chunk_size, sizeof(uint32_t), 1, file);
			hshd_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
			fread(
				uaudio::utils::add(&hshd_chunk, sizeof(uaudio::wave_reader::ChunkHeader)),
				hshd_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader),
				1,
				file
			);
			return hshd_chunk;
		}

		/// <summary>
		/// Returns a sdat chunk.
		/// </summary>
		/// <param name="file">The file pointer.</param>
		/// <returns></returns>
		SDAT_Chunk getSDATChunk(FILE* file)
		{
			SDAT_Chunk sdat_chunk = SDAT_Chunk();
			unsigned char chunk_size[sizeof(uint32_t)];
			fread(&sdat_chunk, uaudio::wave_reader::CHUNK_ID_SIZE, 1, file);
			fread(&chunk_size, sizeof(uint32_t), 1, file);
			sdat_chunk.chunkSize = utils::big_to_little_endian(chunk_size);
			sdat_chunk.data = reinterpret_cast<unsigned char*>(malloc(sdat_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader)));
			fread(
				sdat_chunk.data,
				sdat_chunk.chunkSize - sizeof(uaudio::wave_reader::ChunkHeader),
				1,
				file
			);
			return sdat_chunk;
		}

		int decompile(std::string path)
		{
			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];

			fileContainer.Clear();
			form->fileView->Nodes->Clear();

			FILE* file = nullptr;

			form->filenameContainer->Text = gcnew System::String(path.c_str());

			// Open the file.
			fopen_s(&file, path.c_str(), "rb");
			if (file == nullptr)
			{
				std::cout << "Cannot find file \"" << path << "\"." << std::endl;
				return 1;
			}

			uaudio::wave_reader::ChunkHeader rootChunk;
			getChunk(rootChunk, file);

			uint32_t i = 0;
			while (ftell(file) < rootChunk.chunkSize)
			{
				uaudio::wave_reader::ChunkHeader chunk;
				getChunk(chunk, file);

				fseek(file, -sizeof(uaudio::wave_reader::ChunkHeader), SEEK_CUR);

				if (utils::chunkcmp(reinterpret_cast<char*>(chunk.chunk_id), "SGHD"))
				{
					SGHD_Chunk sghd = getSGHDChunk(file);
				}
				else if (utils::chunkcmp(reinterpret_cast<char*>(chunk.chunk_id), "SGEN"))
				{
					SGEN_Chunk sgen = getSGENChunk(file);
				}
				// DIGI is always a song, so we'll hardcode other chunks.
				else if (utils::chunkcmp(reinterpret_cast<char*>(chunk.chunk_id), "DIGI"))
				{
					DIGI_Chunk digi = getDIGIChunk(file);
					HSHD_Chunk hshd = getHSHDChunk(file);
					SDAT_Chunk sdat = getSDATChunk(file);
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
				else
				{
					fseek(file, chunk.chunkSize, SEEK_CUR);
				}

				int tell = ftell(file);
				float progress = 100.0f / rootChunk.chunkSize * tell;
				form->toolProgressBar->Value = progress;
			}

			MessageBox::Show("Successfully decompiled file", "Decompilation successful", MessageBoxButtons::OK, MessageBoxIcon::Information);
			form->toolProgressBar->Value = 0;
		}
	}
}
