#include "Humongous/Entry.h"

#include <cstdint>
#include <windows.h>
#include <iostream>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/WaveReader.h>
#include <cstdint>
#include <cstdio>
#include <string>

namespace HumongousFileEditor
{
	void Entry::OpenSave()
	{
		OPENFILENAME ofn;

		ZeroMemory(&ofn, sizeof(ofn));

		std::string name = std::to_string(num) + GetCommonExtension();
		wchar_t wtext[260];
		ZeroMemory(wtext, 260);
		mbstowcs(wtext, name.c_str(), name.length());
		ofn.lpstrFile = wtext;
		ofn.lStructSize = sizeof(ofn);
		ofn.nMaxFile = MAX_PATH;

		std::string filter = GetFilter();
		wchar_t wfilter[260];
		ZeroMemory(wfilter, 260);
		mbstowcs(wfilter, filter.c_str(), filter.length());
		ofn.lpstrFilter = wfilter;

		ofn.nFilterIndex = 1;
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrFileTitle = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

		if (GetSaveFileName(&ofn))
		{
			const auto path = new char[wcslen(ofn.lpstrFile) + 1];
			wsprintfA(path, "%S", ofn.lpstrFile);

			std::string pathString = std::string(path);

			Save(pathString);

			delete[] path;
		}
	}

	void Entry::OpenReplace()
	{
		OPENFILENAME ofn;

		ZeroMemory(&ofn, sizeof(ofn));

		std::string name = std::to_string(num) + GetCommonExtension();
		wchar_t wname[260];
		ZeroMemory(wname, 260);
		mbstowcs(wname, name.c_str(), name.length());
		ofn.lpstrFile = wname;
		ofn.lStructSize = sizeof(ofn);
		ofn.nMaxFile = MAX_PATH;

		std::string filter = GetFilter();
		wchar_t wfilter[260];
		ZeroMemory(wfilter, 260);
		mbstowcs(wfilter, filter.c_str(), filter.length());
		ofn.lpstrFilter = wfilter;

		ofn.nFilterIndex = 1;
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrFileTitle = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

		if (GetOpenFileNameW(&ofn))
		{
			const auto path = new char[wcslen(ofn.lpstrFile) + 1];
			wsprintfA(path, "%S", ofn.lpstrFile);

			std::string pathString = std::string(path);

			Replace(pathString);

			delete[] path;
		}
	}

	void SongEntry::Save(std::string path)
	{
		FILE* newFile = nullptr;
		errno_t t = fopen_s(&newFile, path.c_str(), "wb");
		if (newFile == nullptr)
		{
			std::cout << "Cannot save file \"" << path.c_str() << "\"." << std::endl;
			return;
		}

		// Construct the riff chunk.
		uaudio::wave_reader::RIFF_Chunk riff_chunk;
		memcpy(riff_chunk.chunk_id, uaudio::wave_reader::RIFF_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
		riff_chunk.chunkSize = sizeof(uaudio::wave_reader::RIFF_Chunk) + size + sizeof(uaudio::wave_reader::FMT_Chunk);
		memcpy(riff_chunk.format, uaudio::wave_reader::RIFF_CHUNK_FORMAT, uaudio::wave_reader::CHUNK_ID_SIZE);
		fwrite(&riff_chunk, sizeof(uaudio::wave_reader::RIFF_Chunk), 1, newFile);

		// Construct the fmt chunk.
		uaudio::wave_reader::FMT_Chunk fmt_chunk;
		memcpy(fmt_chunk.chunk_id, uaudio::wave_reader::FMT_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
		fmt_chunk.chunkSize = sizeof(uaudio::wave_reader::FMT_Chunk) - sizeof(uaudio::wave_reader::ChunkHeader);
		fmt_chunk.audioFormat = 1;
		fmt_chunk.numChannels = 1;
		fmt_chunk.sampleRate = sample_rate;
		fmt_chunk.byteRate = sample_rate;
		fmt_chunk.bitsPerSample = 8;
		fmt_chunk.blockAlign = fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8;
		fwrite(&fmt_chunk, sizeof(uaudio::wave_reader::FMT_Chunk), 1, newFile);

		// Construct the data chunk.
		uaudio::wave_reader::DATA_Chunk data_chunk;
		memcpy(data_chunk.chunk_id, uaudio::wave_reader::DATA_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
		data_chunk.chunkSize = size;
		fwrite(&data_chunk, sizeof(uaudio::wave_reader::DATA_Chunk) - sizeof(data_chunk.data), 1, newFile);
		fwrite(data, size, 1, newFile);

		std::cout << "Saved " << path.c_str() << "." << std::endl;

		fclose(newFile);
	}

	void SongEntry::Replace(std::string path)
	{
		size_t size = 0;
		uaudio::wave_reader::WaveReader::FTell(path.c_str(), size);

		void* allocated_space = malloc(size);
		uaudio::wave_reader::ChunkCollection chunkCollection = uaudio::wave_reader::ChunkCollection(allocated_space, size);

		// Use the allocated memory and also pass the size.
		uaudio::wave_reader::WaveReader::LoadWave(path.c_str(), chunkCollection);

		bool hasChunk = false;
		chunkCollection.HasChunk(hasChunk, uaudio::wave_reader::FMT_CHUNK_ID);
		if (!hasChunk)
		{
			System::Windows::Forms::MessageBox::Show("Sound file has no format data", "Cannot replace file", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
			return;
		}

		chunkCollection.HasChunk(hasChunk, uaudio::wave_reader::DATA_CHUNK_ID);
		if (!hasChunk)
		{
			System::Windows::Forms::MessageBox::Show("Sound file has no pcm data", "Cannot replace file", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
			return;
		}

		uaudio::wave_reader::FMT_Chunk fmt_chunk;
		chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);

		if (fmt_chunk.sampleRate != 11025)
		{
			System::Windows::Forms::MessageBox::Show("Sound file's sample rate is too high", "Cannot replace file", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
			return;
		}

		if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8)
		{
			System::Windows::Forms::MessageBox::Show("Sound file's bits per sample is too high", "Cannot replace file", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
			return;
		}

		uaudio::wave_reader::DATA_Chunk data_chunk;
		chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);

		// Free old data.
		free(data);

		size = data_chunk.chunkSize;
		data = reinterpret_cast<unsigned char*>(malloc(size));
		memcpy(data, data_chunk.data, size);
		sample_rate = fmt_chunk.sampleRate;

		free(allocated_space);
	}

    void TalkieEntry::Save(std::string path)
    {
		FILE* newFile = nullptr;
		errno_t t = fopen_s(&newFile, path.c_str(), "wb");
		if (newFile == nullptr)
		{
			std::cout << "Cannot save file \"" << path.c_str() << "\"." << std::endl;
			return;
		}

		// Construct the riff chunk.
		uaudio::wave_reader::RIFF_Chunk riff_chunk;
		memcpy(riff_chunk.chunk_id, uaudio::wave_reader::RIFF_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
		riff_chunk.chunkSize = sizeof(uaudio::wave_reader::RIFF_Chunk) + size + sizeof(uaudio::wave_reader::FMT_Chunk);
		memcpy(riff_chunk.format, uaudio::wave_reader::RIFF_CHUNK_FORMAT, uaudio::wave_reader::CHUNK_ID_SIZE);
		fwrite(&riff_chunk, sizeof(uaudio::wave_reader::RIFF_Chunk), 1, newFile);

		// Construct the fmt chunk.
		uaudio::wave_reader::FMT_Chunk fmt_chunk;
		memcpy(fmt_chunk.chunk_id, uaudio::wave_reader::FMT_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
		fmt_chunk.chunkSize = sizeof(uaudio::wave_reader::FMT_Chunk) - sizeof(uaudio::wave_reader::ChunkHeader);
		fmt_chunk.audioFormat = 1;
		fmt_chunk.numChannels = 1;
		fmt_chunk.sampleRate = sample_rate;
		fmt_chunk.byteRate = sample_rate;
		fmt_chunk.bitsPerSample = 8;
		fmt_chunk.blockAlign = fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8;
		fwrite(&fmt_chunk, sizeof(uaudio::wave_reader::FMT_Chunk), 1, newFile);

		// Construct the data chunk.
		uaudio::wave_reader::DATA_Chunk data_chunk;
		memcpy(data_chunk.chunk_id, uaudio::wave_reader::DATA_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
		data_chunk.chunkSize = size;
		fwrite(&data_chunk, sizeof(uaudio::wave_reader::DATA_Chunk) - sizeof(data_chunk.data), 1, newFile);
		fwrite(data, size, 1, newFile);

		std::cout << "Saved " << path.c_str() << "." << std::endl;

		fclose(newFile);
    }
}