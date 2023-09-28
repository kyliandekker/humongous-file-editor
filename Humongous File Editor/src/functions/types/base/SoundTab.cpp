#include "functions/types/base/SoundTab.h"

#include <wtypes.h>
#include <commdlg.h>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/ChunkCollection.h>

#include "lowlevel/utils.h"
#include "systems/Logger.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"

namespace HumongousFileEditor
{
    bool SoundTab::ReplaceResource(std::string& file_path, uaudio::wave_reader::FMT_Chunk fmt_chunk, uaudio::wave_reader::DATA_Chunk data_chunk)
    {
		OPENFILENAME ofn;
		TCHAR sz_file[260] = { 0 };

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = sz_file;
		ofn.nMaxFile = sizeof(sz_file);
		ofn.lpstrFilter = L"\
						WAVE file (*.wav)\
						\0*.WAV;*.wav\0";
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileNameW(&ofn))
		{
			const auto path = new char[wcslen(ofn.lpstrFile) + 1];
			wsprintfA(path, "%S", ofn.lpstrFile);

			file_path = std::string(path);

			if (!utils::ends_with(path, ".wav"))
				file_path += ".wav";

			delete[] path;

			size_t wave_size = 0;
			if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::FTell(file_path.c_str(), wave_size)))
				return false;

			uaudio::wave_reader::ChunkCollection chunkCollection(malloc(wave_size), wave_size);
			if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::LoadWave(file_path.c_str(), chunkCollection)))
				return false;

			uaudio::wave_reader::DATA_Chunk data_chunk;
			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
				return false;

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
				return false;

			if (fmt_chunk.byteRate != 11025)
				return false;

			if (fmt_chunk.sampleRate != 11025)
				return false;

			if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8)
				return false;

			if (fmt_chunk.numChannels != uaudio::wave_reader::WAVE_CHANNELS_MONO)
				return false;

			return true;
		}

		return false;
    }

	bool SoundTab::SaveSound(chunk_reader::HSHD_Chunk& hshd_chunk, chunk_reader::SDAT_Chunk& sdat_chunk)
	{
		unsigned char* data = sdat_chunk.data;
		size_t chunk_size = sdat_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader) - sizeof(sdat_chunk.data);

		OPENFILENAME ofn;
		TCHAR sz_file[260] = { 0 };

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = sz_file;
		ofn.nMaxFile = sizeof(sz_file);
		ofn.lpstrFilter = L"\
						WAVE file (*.wav)\
						\0*.WAV;*.wav\0";
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetSaveFileNameW(&ofn))
		{
			const auto path = new char[wcslen(ofn.lpstrFile) + 1];
			wsprintfA(path, "%S", ofn.lpstrFile);

			std::string save_path_s = std::string(path);
			
			delete[] path;

			if (!utils::ends_with(save_path_s, ".wav"))
				save_path_s += ".wav";

			FILE* file = nullptr;
			fopen_s(&file, save_path_s.c_str(), "wb");
			if (file == nullptr)
			{
				LOGF(logger::LOGSEVERITY_ERROR, "Could not export resource.");
				return false;
			}

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			fmt_chunk.audioFormat = uaudio::wave_reader::WAV_FORMAT_UNCOMPRESSED;
			fmt_chunk.bitsPerSample = uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8;
			fmt_chunk.blockAlign = uaudio::wave_reader::BLOCK_ALIGN_8_BIT_MONO;
			fmt_chunk.sampleRate = hshd_chunk.sample_rate;
			fmt_chunk.numChannels = uaudio::wave_reader::WAVE_CHANNELS_MONO;
			fmt_chunk.byteRate = hshd_chunk.sample_rate;
			fmt_chunk.chunkSize = sizeof(uaudio::wave_reader::FMT_Chunk) - sizeof(uaudio::wave_reader::ChunkHeader);
			memcpy(fmt_chunk.chunk_id, uaudio::wave_reader::FMT_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);

			uaudio::wave_reader::DATA_Chunk data_chunk;
			memcpy(data_chunk.chunk_id, uaudio::wave_reader::DATA_CHUNK_ID, uaudio::wave_reader::CHUNK_ID_SIZE);
			data_chunk.chunkSize = static_cast<uint32_t>(chunk_size);
			data_chunk.data = data;

			uaudio::wave_reader::RIFF_Chunk riff_chunk;
			memcpy(riff_chunk.chunk_id, uaudio::wave_reader::RIFF_CHUNK_ID, CHUNK_ID_SIZE);
			riff_chunk.chunkSize =
				sizeof(uaudio::wave_reader::FMT_Chunk) +
				(sizeof(uaudio::wave_reader::RIFF_Chunk) - sizeof(uaudio::wave_reader::ChunkHeader)) +
				sizeof(uaudio::wave_reader::DATA_Chunk) - sizeof(data_chunk.data) +
				static_cast<uint32_t>(chunk_size);
			memcpy(riff_chunk.format, uaudio::wave_reader::RIFF_CHUNK_FORMAT, uaudio::wave_reader::CHUNK_ID_SIZE);

			fwrite(&riff_chunk, sizeof(riff_chunk), 1, file);
			fwrite(&fmt_chunk, sizeof(fmt_chunk), 1, file);
			fwrite(&data_chunk, sizeof(uaudio::wave_reader::ChunkHeader), 1, file);
			fwrite(data, chunk_size, 1, file);

			fclose(file);

			return true;
		}

		return false;
	}
}