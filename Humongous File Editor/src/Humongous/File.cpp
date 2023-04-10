#include "Humongous/File.h"

#include <cstdint>
#include <windows.h>
#include <iostream>
#include <uaudio_wave_reader/WaveChunks.h>
#include <cstdint>
#include <cstdio>
#include <string>

namespace HumongousFileEditor
{
	void File::OpenSave()
	{
		OPENFILENAME ofn;
		TCHAR sz_file[260] = { 0 };

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);

		std::string name = GetCommonExtension();
		wchar_t wtext[260];
		ZeroMemory(wtext, 260);
		mbstowcs(wtext, name.c_str(), name.length());
		ofn.lpstrFile = wtext;
		ofn.lStructSize = sizeof(ofn);
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"Microsoft wave file (*.wav)\0*.wav\0";
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

	void SongFile::Save(std::string path)
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

    void TalkieFile::Save(std::string path)
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