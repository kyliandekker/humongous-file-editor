#include "functions/TabFunctions.h"

#include <bitset>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <windows.h>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/WaveReader.h>
#include <sstream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "lowlevel/HumongousChunks.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include "file/Files.h"
#include "forms/HumongousButton.h"
#include "systems/AudioSystem.h"
#include "file/BMPHeader.h"
#include "forms/HumongousNode.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "systems/Logger.h"
#include "file/ResourceType.h"
#include <HumongousEditorForm.h>

namespace HumongousFileEditor
{
	constexpr int LOOKUP_LIMIT = 15;

	/// <summary>
	/// Returns the offset of a chunk (looking from offset)
	/// </summary>
	/// <param name="fc">The file.</param>
	/// <param name="offset">The offset from where to look.</param>
	/// <param name="chunk_name">The chunk ID.</param>
	/// <returns>Offset if found, -1 if not found.</returns>
	size_t getOffsetChunk(chunk_reader::FileContainer*& fc, size_t offset, std::vector<std::string> chunk_names)
	{
		chunk_reader::ChunkInfo info = fc->GetChunkInfo(offset);
		info = fc->GetNextChunk(info.offset);

		int i = 0;
		while (i < LOOKUP_LIMIT && offset < fc->size)
		{
			for (size_t j = 0; j < chunk_names.size(); j++)
				if (utils::chunkcmp(info.chunk_id, chunk_names[j].c_str()) == 0)
					return info.offset;
			info = fc->GetNextChunk(info.offset);
		}

		return -1;
	}
	/// <summary>
	/// Returns the offset of a chunk (looking from offset)
	/// </summary>
	/// <param name="fc">The file.</param>
	/// <param name="offset">The offset from where to look.</param>
	/// <param name="chunk_name">The chunk ID.</param>
	/// <returns>Offset if found, -1 if not found.</returns>
	size_t getOffsetChunkWithOffset(chunk_reader::FileContainer*& fc, size_t offset, size_t max_offset, std::vector<std::string> chunk_names)
	{
		chunk_reader::ChunkInfo info = fc->GetChunkInfo(offset);
		info = fc->GetNextChunk(info.offset);

		int i = 0;
		while (i < LOOKUP_LIMIT && offset < max_offset)
		{
			for (size_t j = 0; j < chunk_names.size(); j++)
				if (utils::chunkcmp(info.chunk_id, chunk_names[j].c_str()) == 0)
					return info.offset;
			info = fc->GetNextChunk(info.offset);
		}

		return -1;
	}
	// Adds a row of info to the info tab.
	void AddInfoRow(System::String^ key, System::String^ value, System::Windows::Forms::DataGridView^ propertyGrid, float& posX, float& posY)
	{
		int i = propertyGrid->Rows->Add(key, value);
		propertyGrid->Rows[i]->ReadOnly = true;
	}

	// Callback for the play button.
	System::Void TabFunctions::PlayButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		HumongousButton^ btn = (HumongousButton^)sender;

		chunk_reader::FileContainer* fc = files::FILES.getFile(btn->fileType);

		if (fc == nullptr)
			return;

		chunk_reader::SDAT_Chunk sdat_chunk;
		chunk_reader::HSHD_Chunk hshd_chunk;
		bool has_sdat = false;
		chunk_reader::ChunkInfo chunk = fc->GetChunkInfo(btn->offset);
		if (utils::chunkcmp(chunk.chunk_id, chunk_reader::SGEN_CHUNK_ID) == 0)
			has_sdat = SongTab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0)
			has_sdat = TalkieTab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::DIGI_CHUNK_ID) == 0)
			has_sdat = DigiTab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);

		if (!has_sdat)
		{
			LOGF(logger::LOGSEVERITY_ERROR, "Could not process file type of file \"%s\" because it has no SDAT chunk.", fc->path.c_str());
			return;
		}

		audioSystem.Stop();
		audioSystem.Play(sdat_chunk.data, sdat_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader) - sizeof(sdat_chunk.data));
	}
	// Callback for the stop button.
	System::Void TabFunctions::StopButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		audioSystem.Stop();
	}
	// Callback for the export button.
	System::Void TabFunctions::ExportButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		HumongousButton^ btn = (HumongousButton^)sender;

		chunk_reader::FileContainer* fc = files::FILES.getFile(btn->fileType);

		if (fc == nullptr)
			return;

		chunk_reader::ChunkInfo chunk = fc->GetChunkInfo(btn->offset);
		files::ResourceType type = files::GetResourceTypeByChunkName(chunk.chunk_id);
		switch (type)
		{
			case files::ResourceType::Song:
			case files::ResourceType::Talkie:
			case files::ResourceType::SFX:
			{
				chunk_reader::SDAT_Chunk sdat_chunk;
				chunk_reader::HSHD_Chunk hshd_chunk;
				bool has_sdat = false;
				chunk_reader::ChunkInfo chunk = fc->GetChunkInfo(btn->offset);
				if (utils::chunkcmp(chunk.chunk_id, chunk_reader::SGEN_CHUNK_ID) == 0)
					has_sdat = SongTab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);
				else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0)
					has_sdat = TalkieTab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);
				else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::DIGI_CHUNK_ID) == 0)
					has_sdat = DigiTab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);

				if (!has_sdat)
				{
					LOGF(logger::LOGSEVERITY_ERROR, "Could not process file type of file \"%s\" because it has no SDAT chunk.", fc->path.c_str());
					return;
				}

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

					if (!utils::ends_with(save_path_s, ".wav"))
						save_path_s += ".wav";

					FILE* file = nullptr;
					fopen_s(&file, save_path_s.c_str(), "wb");
					if (file == nullptr)
					{
						LOGF(logger::LOGSEVERITY_ERROR, "Could not export file \"%s\".", fc->path.c_str());
						return;
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
				}
				break;
			}
			case files::ResourceType::Script:
			{
				break;
			}
			case files::ResourceType::RoomBackground:
			{
				img_info info;
				if (!RoomBackgroundTab::GetData(fc, btn->offset, info))
					return;

				OPENFILENAME ofn;
				TCHAR sz_file[260] = { 0 };

				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = sz_file;
				ofn.nMaxFile = sizeof(sz_file);
				ofn.lpstrFilter = L"\
						BMP file (*.bmp)\
						\0*.BMP;*.bmp\0\
						PNG file (*.png)\
						\0*.PNG;*.png\0";
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = nullptr;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (GetSaveFileNameW(&ofn))
				{
					const auto path = new char[wcslen(ofn.lpstrFile) + 1];
					wsprintfA(path, "%S", ofn.lpstrFile);
					std::string save_path_s = std::string(path);

					if (ofn.nFilterIndex == 1)
					{
						if (!utils::ends_with(save_path_s, ".bmp"))
							save_path_s += ".bmp";
						stbi_write_bmp(save_path_s.c_str(), static_cast<int>(info.width), static_cast<int>(info.height), static_cast<int>(info.channels), info.data);
					}
					else
					{
						if (!utils::ends_with(save_path_s, ".png"))
							save_path_s += ".png";
						stbi_write_png(save_path_s.c_str(), static_cast<int>(info.width), static_cast<int>(info.height), static_cast<int>(info.channels), info.data, info.width * info.channels);
					}

					free(info.data);
				}

				break;
			}
			default:
			{
				break;
			}
		}
	}
	// Callback for the export button.
	System::Void TabFunctions::ReplaceSongButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		HumongousButton^ btn = (HumongousButton^)sender;

		chunk_reader::FileContainer* fc = files::FILES.getFile(btn->fileType);

		if (fc == nullptr)
			return;

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

			std::string save_path_s = std::string(path);

			if (!utils::ends_with(save_path_s, ".wav"))
				save_path_s += ".wav";

			size_t wave_size = 0;
			if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::FTell(save_path_s.c_str(), wave_size)))
				return;

			uaudio::wave_reader::ChunkCollection chunkCollection(malloc(wave_size), wave_size);
			if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::LoadWave(save_path_s.c_str(), chunkCollection)))
				return;

			uaudio::wave_reader::DATA_Chunk data_chunk;
			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
				return;

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
				return;

			if (fmt_chunk.byteRate != 11025)
				return;

			if (fmt_chunk.sampleRate != 11025)
				return;

			if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8)
				return;

			if (fmt_chunk.numChannels != uaudio::wave_reader::WAVE_CHANNELS_MONO)
				return;

			// Get SGEN chunk first (tells us the position of the SONG).
			chunk_reader::SGEN_Chunk sgen_chunk;
			memcpy(&sgen_chunk, utils::add(fc->data, btn->offset), sizeof(chunk_reader::SGEN_Chunk));

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
			form->entryView->Nodes->Clear();

			chunk_reader::SDAT_Chunk sdat_chunk;

			// Get SDAT chunk for the raw audio data.
			size_t sdat_offset = getOffsetChunk(fc, sgen_chunk.song_pos, { chunk_reader::SDAT_CHUNK_ID });
			if (sdat_offset == -1)
				return;

			size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
			memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
			sdat_chunk.SetChunkSize(data_chunk.chunkSize + header_size);

			unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(header_size + data_chunk.chunkSize));
			memcpy(new_data, &sdat_chunk, header_size);
			memcpy(utils::add(new_data, header_size), data_chunk.data, data_chunk.chunkSize);

			fc->Replace(sdat_offset, new_data, sdat_chunk.ChunkSize());

			// AFTER PROCESSING:
			//// TODO: Maybe do this specifically for replacing a song in the replace method.
			//// There is a chance this was a song chunk. If so, change the offset of the SGEN and the SGENS following (or before).
			//if (utils::chunkcmp(chunk.chunk_id, DIGI_CHUNK_ID) == 0)
			//{
			//	bool past = false;

			//	next_chunk = GetChunkInfo(sizeof(HumongousHeader));
			//	while (next_chunk.offset < size)
			//	{
			//		if (utils::chunkcmp(next_chunk.chunk_id, SGEN_CHUNK_ID) == 0)
			//		{
			//			SGEN_Chunk* sgen_chunk = reinterpret_cast<SGEN_Chunk*>(utils::add(new_data, next_chunk.offset));
			//			if (sgen_chunk->song_pos == offset)
			//			{
			//				sgen_chunk->song_pos = offset;
			//				sgen_chunk->song_size = new_size;
			//				past = true;
			//			}
			//			else if (past)
			//			{
			//				sgen_chunk->song_pos = offset += dif_size;
			//			}
			//		}
			//		next_chunk = GetNextChunk(next_chunk.offset);
			//	}
			//}

			HumongousFileEditor::chunk_reader::ResourceGatherer rg;
			rg.ReadHE4(fc);

			free(new_data);
		}
	}
	void TabFunctions::AddTab(HumongousNode^ node, System::Windows::Forms::TabControl^ tabControl)
	{
		chunk_reader::FileContainer* fc = files::FILES.getFile(node->fileType);

		if (fc == nullptr)
			return;

		chunk_reader::ChunkInfo chunk = fc->GetChunkInfo(node->offset);
		files::ResourceType type = files::GetResourceTypeByChunkName(chunk.chunk_id);

		if (type == files::ResourceType::Unknown)
			return;

		// Construct tab.
		System::Windows::Forms::TabPage^ newTab;
		newTab = (gcnew System::Windows::Forms::TabPage());
		newTab->SuspendLayout();

		newTab = (gcnew System::Windows::Forms::TabPage());
		newTab->Location = System::Drawing::Point(4, 25);
		newTab->Name = node->Name;
		newTab->Padding = System::Windows::Forms::Padding(10, 3, 3, 3);
		newTab->Size = System::Drawing::Size(659, 396);
		newTab->TabIndex = 0;
		newTab->Text = node->Text;
		newTab->UseVisualStyleBackColor = true;

		// Construct action panel (button area)
		System::Windows::Forms::Panel^ propertyPanel = gcnew System::Windows::Forms::Panel();
		propertyPanel->BackColor = System::Drawing::Color::WhiteSmoke;
		newTab->Controls->Add(propertyPanel);
		propertyPanel->Dock = System::Windows::Forms::DockStyle::Fill;
		propertyPanel->Location = System::Drawing::Point(0, 0);
		propertyPanel->Name = L"propertyPanel1";
		propertyPanel->Size = System::Drawing::Size(497, 462);
		propertyPanel->TabIndex = 0;

		System::Windows::Forms::DataGridView^ propertyGrid = (gcnew System::Windows::Forms::DataGridView());
		propertyPanel->Controls->Add(propertyGrid);
		propertyGrid->Dock = System::Windows::Forms::DockStyle::Fill;
		propertyGrid->Location = System::Drawing::Point(0, 0);
		propertyGrid->Name = L"propertyGrid1";
		propertyGrid->TabIndex = 2;
		propertyGrid->Columns->Add(gcnew System::String("Property Name"), gcnew System::String("Property Name"));
		propertyGrid->Columns->Add(gcnew System::String("Value"), gcnew System::String("Value"));
		propertyGrid->Columns[0]->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
		propertyGrid->Columns[1]->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
		propertyGrid->ReadOnly = true;
		propertyGrid->ColumnHeadersVisible = false;
		propertyGrid->RowHeadersVisible = false;

		// Construct action panel (button area)
		System::Windows::Forms::FlowLayoutPanel^ actionPanel = gcnew System::Windows::Forms::FlowLayoutPanel();
		actionPanel->BackColor = System::Drawing::Color::WhiteSmoke;
		actionPanel->Dock = System::Windows::Forms::DockStyle::Bottom;
		actionPanel->Location = System::Drawing::Point(0, 334);
		actionPanel->Name = L"flowLayoutPanel1";
		actionPanel->Size = System::Drawing::Size(355, 64);
		actionPanel->TabIndex = 0;

		// Construct info area.
		float posX = 35, posY = 35;
		AddInfoRow("Pos", gcnew System::String(std::to_string(node->offset).c_str()), propertyGrid, posX, posY);

		switch (type)
		{
			case files::ResourceType::Song:
			{
				GetSong(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
				break;
			}
			case files::ResourceType::Talkie:
			{
				GetTalk(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
				break;
			}
			case files::ResourceType::SFX:
			{
				GetDigi(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
				break;
			}
			case files::ResourceType::Script:
			{
				GetScrp(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
				break;
			}
			case files::ResourceType::RoomBackground:
			{
				GetRMIM(fc, node->offset, newTab, propertyGrid, actionPanel, propertyPanel, posX, posY);
				break;
			}
			case files::ResourceType::RoomImage:
			{
				//GetOBIM(fc, node->offset, newTab, propertyGrid, actionPanel, propertyPanel, posX, posY);
				break;
			}
			case files::ResourceType::Room:
			{
				GetRNAM(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
				break;
			}
			default:
				return;
		}

		// Construct export button.
		HumongousButton^ exportButton;
		exportButton = (gcnew HumongousButton());

		exportButton->Location = System::Drawing::Point(232, 53);
		exportButton->Name = gcnew System::String("Export_") + gcnew System::String(newTab->Name);
		exportButton->Size = System::Drawing::Size(75, 23);
		exportButton->TabIndex = 2;
		exportButton->offset = node->offset;
		exportButton->fileType = node->fileType;
		exportButton->Text = L"Export";
		exportButton->UseVisualStyleBackColor = true;
		exportButton->Click += gcnew System::EventHandler(this, &TabFunctions::ExportButton_Click);

		exportButton->ResumeLayout(false);
		actionPanel->Controls->Add(exportButton);

		newTab->ResumeLayout(false);

		newTab->Controls->Add(actionPanel);
		tabControl->Controls->Add(newTab);
	}
	void TabFunctions::GetTalk(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Talk"), propertyGrid, posX, posY);

		chunk_reader::SDAT_Chunk sdat_chunk;
		chunk_reader::HSHD_Chunk hshd_chunk;
		if (!TalkieTab::GetData(fc, offset, sdat_chunk, hshd_chunk))
			return;

		AddInfoRow("Sample Rate", gcnew System::String(std::to_string(hshd_chunk.sample_rate).c_str()), propertyGrid, posX, posY);
		AddInfoRow("Size (in bytes)", gcnew System::String(std::to_string(sdat_chunk.ChunkSize()).c_str()), propertyGrid, posX, posY);

		AddSoundButtons(tab, offset, fc->fileType, panel);
	}
	void TabFunctions::GetDigi(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Sfx"), propertyGrid, posX, posY);

		chunk_reader::SDAT_Chunk sdat_chunk;
		chunk_reader::HSHD_Chunk hshd_chunk;
		if (!DigiTab::GetData(fc, offset, sdat_chunk, hshd_chunk))
			return;

		AddInfoRow("Sample Rate", gcnew System::String(std::to_string(hshd_chunk.sample_rate).c_str()), propertyGrid, posX, posY);
		AddInfoRow("Size (in bytes)", gcnew System::String(std::to_string(sdat_chunk.ChunkSize()).c_str()), propertyGrid, posX, posY);

		AddSoundButtons(tab, offset, fc->fileType, panel);
	}
	void TabFunctions::GetSong(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Song"), propertyGrid, posX, posY);

		chunk_reader::SDAT_Chunk sdat_chunk;
		chunk_reader::HSHD_Chunk hshd_chunk;
		if (!SongTab::GetData(fc, offset, sdat_chunk, hshd_chunk))
			return;

		AddInfoRow("Sample Rate", gcnew System::String(std::to_string(hshd_chunk.sample_rate).c_str()), propertyGrid, posX, posY);
		AddInfoRow("Size (in bytes)", gcnew System::String(std::to_string(sdat_chunk.ChunkSize()).c_str()), propertyGrid, posX, posY);

		AddSoundButtons(tab, offset, fc->fileType, panel);

		HumongousButton^ replaceButton;
		replaceButton = (gcnew HumongousButton());

		replaceButton->Location = System::Drawing::Point(232, 53);
		replaceButton->Name = gcnew System::String("ReplaceSong_") + gcnew System::String(tab->Name);
		replaceButton->Size = System::Drawing::Size(75, 23);
		replaceButton->TabIndex = 2;
		replaceButton->Text = L"Replace";
		replaceButton->offset = offset;
		replaceButton->fileType = fc->fileType;
		replaceButton->UseVisualStyleBackColor = true;
		replaceButton->Click += gcnew System::EventHandler(this, &TabFunctions::ReplaceSongButton_Click);

		replaceButton->ResumeLayout(false);
		panel->Controls->Add(replaceButton);
	}
	void TabFunctions::GetScrp(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Script"), propertyGrid, posX, posY);

		chunk_reader::SCRP_Chunk scrp_chunk;
		memcpy(&scrp_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::SCRP_Chunk) - sizeof(scrp_chunk.data));
		scrp_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, offset + sizeof(chunk_reader::HumongousHeader)));

		std::string t = std::string(reinterpret_cast<char*>(scrp_chunk.data));
		AddInfoRow("Script", gcnew System::String(t.c_str()), propertyGrid, posX, posY);
	}
	std::vector<uint8_t> create_bitstream(unsigned char* data, size_t length)
	{
		std::vector<uint8_t> bits;
		for (size_t i = 0; i < length; ++i) {
			char c = data[i];
			for (int j = 0; j < 8; j++)
			{
				bits.push_back((c >> j) & 1);
			}
		}
		return bits;
	}
	uint8_t collect_bits(int& pos, std::vector<uint8_t>& bitstream, int count)
	{
		// TODO: check if special handling needed when count > 8
		assert(count <= 8);

		int result = 0;
		for (int i = 0; i < count; i++)
			result |= bitstream[pos++] << i;

		return result;
	}
	void TabFunctions::GetRMIM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY)
	{
		size_t next_offset = getOffsetChunk(fc, offset, 
			{
				chunk_reader::IM00_CHUNK_ID, 
				chunk_reader::IM01_CHUNK_ID, 
				chunk_reader::IM02_CHUNK_ID, 
				chunk_reader::IM03_CHUNK_ID, 
				chunk_reader::IM04_CHUNK_ID, 
				chunk_reader::IM05_CHUNK_ID, 
				chunk_reader::IM06_CHUNK_ID, 
				chunk_reader::IM07_CHUNK_ID, 
				chunk_reader::IM08_CHUNK_ID, 
				chunk_reader::IM09_CHUNK_ID, 
				chunk_reader::IM10_CHUNK_ID,
				chunk_reader::IM11_CHUNK_ID,
				chunk_reader::IM12_CHUNK_ID,
				chunk_reader::IM13_CHUNK_ID,
				chunk_reader::IM14_CHUNK_ID,
				chunk_reader::IM15_CHUNK_ID,
				chunk_reader::IM16_CHUNK_ID,
				chunk_reader::IM17_CHUNK_ID,
				chunk_reader::IM0A_CHUNK_ID,
				chunk_reader::IM0B_CHUNK_ID,
				chunk_reader::IM0C_CHUNK_ID,
				chunk_reader::IM0D_CHUNK_ID,
				chunk_reader::IM0E_CHUNK_ID,
				chunk_reader::IM0F_CHUNK_ID
			});
		if (next_offset == -1)
			return;

		AddInfoRow("Type", gcnew System::String("Room Background"), propertyGrid, posX, posY);

		// Get RMHD chunk for width and height of image.
		size_t rmhd_offset = getOffsetChunk(fc, offset, { chunk_reader::RMHD_CHUNK_ID });
		if (rmhd_offset == -1)
			return;

		chunk_reader::RMHD_Chunk rmhd_chunk;
		memcpy(&rmhd_chunk, utils::add(fc->data, rmhd_offset), sizeof(chunk_reader::RMHD_Chunk));
		unsigned char* d = reinterpret_cast<unsigned char*>(utils::add(fc->data, rmhd_offset));

		AddInfoRow("Width", gcnew System::String(std::to_string(rmhd_chunk.width).c_str()), propertyGrid, posX, posY);
		AddInfoRow("Height", gcnew System::String(std::to_string(rmhd_chunk.height).c_str()), propertyGrid, posX, posY);

		img_info info;
		if (!RoomBackgroundTab::GetData(fc, offset, info))
			return;

		propertyGrid->Dock = System::Windows::Forms::DockStyle::Top;
		propertyGrid->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

		System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(static_cast<int>(info.width), static_cast<int>(info.height));

		int cur = 0;
		for (size_t i = 0; i < info.size; i += info.channels, cur++)
		{
			int y = cur / static_cast<int>(info.width);
			int x = cur % static_cast<int>(info.width);
			if (info.channels < 4)
				bmp->SetPixel(x, y, System::Drawing::Color::FromArgb(255, info.data[i], info.data[i + 1], info.data[i + 2]));
			else
				bmp->SetPixel(x, y, System::Drawing::Color::FromArgb(info.data[i + 3], info.data[i], info.data[i + 1], info.data[i + 2]));
		}

		System::Windows::Forms::PictureBox^ pictureBox;
		pictureBox = (gcnew System::Windows::Forms::PictureBox());
		pictureBox->Dock = System::Windows::Forms::DockStyle::Top;
		pictureBox->Location = System::Drawing::Point(0, propertyGrid->Height);
		pictureBox->Name = L"Action Panel";
		float relativeW = 1.0f / bmp->Width * tab->Width;
		pictureBox->Image = bmp;
		pictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
		pictureBox->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

		propertyPanel->Controls->Add(pictureBox);
		free(info.data);
	}
	//void TabFunctions::GetOBIM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY)
	//{
	//	AddInfoRow("Type", gcnew System::String("Room Image"), propertyGrid, posX, posY);

	//	// Get IMHD chunk for width and height of image.
	//	size_t imhd_offset = getOffsetChunk(fc, offset, { chunk_reader::IMHD_CHUNK_ID });
	//	if (imhd_offset == -1)
	//		return;

	//	chunk_reader::IMHD_Chunk imhd_chunk;
	//	memcpy(&imhd_chunk, utils::add(fc->data, imhd_offset), sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data));
	//	imhd_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, imhd_offset + (sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data))));

	//	AddInfoRow("Width", gcnew System::String(std::to_string(imhd_chunk.width).c_str()), propertyGrid, posX, posY);
	//	AddInfoRow("Height", gcnew System::String(std::to_string(imhd_chunk.height).c_str()), propertyGrid, posX, posY);

	//	std::vector<img_info> infos;
	//	if (!RoomImageTab::GetData(fc, offset, infos))
	//		return;

	//	img_info& info = infos[0];

	//	propertyGrid->Dock = System::Windows::Forms::DockStyle::Top;
	//	propertyGrid->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

	//	System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(static_cast<int>(imhd_chunk.width), static_cast<int>(info.height));

	//	int cur = 0;
	//	for (size_t i = 0; i < info.size; i += info.channels, cur++)
	//	{
	//		int y = cur / static_cast<int>(imhd_chunk.width);
	//		int x = cur % static_cast<int>(imhd_chunk.width);
	//		if (info.channels < 4)
	//			bmp->SetPixel(x, y, System::Drawing::Color::FromArgb(255, info.data[i], info.data[i + 1], info.data[i + 2]));
	//		else
	//			bmp->SetPixel(x, y, System::Drawing::Color::FromArgb(info.data[i + 3], info.data[i], info.data[i + 1], info.data[i + 2]));
	//	}

	//	System::Windows::Forms::PictureBox^ pictureBox;
	//	pictureBox = (gcnew System::Windows::Forms::PictureBox());
	//	pictureBox->Dock = System::Windows::Forms::DockStyle::Top;
	//	pictureBox->Location = System::Drawing::Point(0, propertyGrid->Height);
	//	pictureBox->Name = L"Action Panel";
	//	float relativeW = 1.0f / bmp->Width * tab->Width;
	//	pictureBox->Image = bmp;
	//	pictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
	//	pictureBox->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

	//	propertyPanel->Controls->Add(pictureBox);
	//	free(info.data);
	//}
	void TabFunctions::GetRNAM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Rooms"), propertyGrid, posX, posY);

		chunk_reader::DISK_Chunk disk_chunk;
		memcpy(&disk_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::DISK_Chunk) - sizeof(disk_chunk.data));

		// Get RNAM chunk for the sample rate.
		size_t rnam_offset = getOffsetChunk(fc, offset, { chunk_reader::RNAM_CHUNK_ID });
		if (rnam_offset == -1)
			return;

		chunk_reader::RNAM_Chunk* rnam_chunk = reinterpret_cast<chunk_reader::RNAM_Chunk*>(utils::add(fc->data, rnam_offset));
		size_t rnam_end = rnam_offset + rnam_chunk->ChunkSize();
		size_t pos = rnam_offset + sizeof(chunk_reader::HumongousHeader) + sizeof(uint16_t);
		std::string room_name;

		std::vector<std::string> room_names;
		while (pos < rnam_end)
		{
			unsigned char ch;
			memcpy(&ch, utils::add(fc->data, pos), sizeof(char));
			if (utils::unsignedCharCmp(ch, '\0'))
			{
				room_names.push_back(room_name);
				room_name = "";
				pos += sizeof(uint16_t);
			}
			else
				room_name += ch;
			pos++;
		}

		AddInfoRow("Number of Rooms", gcnew System::String(std::to_string(room_names.size()).c_str()), propertyGrid, posX, posY);

		for (size_t i = 0; i < room_names.size(); i++)
			AddInfoRow(gcnew System::String(std::to_string(i).c_str()), gcnew System::String(room_names[i].c_str()), propertyGrid, posX, posY);
	}
	void TabFunctions::AddSoundButtons(System::Windows::Forms::TabPage^ tab, size_t offset, files::FileType fileType, System::Windows::Forms::Panel^ panel)
	{
		HumongousButton^ playButton;
		playButton = (gcnew HumongousButton());

		playButton->Location = System::Drawing::Point(232, 53);
		playButton->Name = gcnew System::String("Play_") + gcnew System::String(tab->Name);
		playButton->Size = System::Drawing::Size(75, 23);
		playButton->TabIndex = 2;
		playButton->Text = L"Play";
		playButton->offset = offset;
		playButton->fileType = fileType;
		playButton->UseVisualStyleBackColor = true;
		playButton->Click += gcnew System::EventHandler(this, &TabFunctions::PlayButton_Click);

		HumongousButton^ stopButton;
		stopButton = (gcnew HumongousButton());

		stopButton->Location = System::Drawing::Point(232, 53);
		stopButton->Name = gcnew System::String("Stop_") + gcnew System::String(tab->Name);
		stopButton->Size = System::Drawing::Size(75, 23);
		stopButton->TabIndex = 2;
		stopButton->Text = L"Stop";
		stopButton->UseVisualStyleBackColor = true;
		stopButton->Click += gcnew System::EventHandler(this, &TabFunctions::StopButton_Click);

		playButton->ResumeLayout(false);
		stopButton->ResumeLayout(false);

		panel->Controls->Add(playButton);
		panel->Controls->Add(stopButton);
	}

	bool TalkieTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk)
	{
		// Get HSHD chunk for the sample rate.
		size_t hshd_offset = getOffsetChunk(fc, offset, { chunk_reader::HSHD_CHUNK_ID });
		if (hshd_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		size_t sdat_offset = getOffsetChunk(fc, offset, { chunk_reader::SDAT_CHUNK_ID });
		if (sdat_offset == -1)
			return false;

		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, sdat_offset + header_size));
		
		return true;
	}

	bool SongTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk)
	{
		// Get SGEN chunk first (tells us the position of the SONG).
		chunk_reader::SGEN_Chunk sgen_chunk;
		memcpy(&sgen_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::SGEN_Chunk));

		// Get HSHD chunk for the sample rate.
		size_t hshd_offset = getOffsetChunk(fc, sgen_chunk.song_pos, { chunk_reader::HSHD_CHUNK_ID });
		if (hshd_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		size_t sdat_offset = getOffsetChunk(fc, sgen_chunk.song_pos, { chunk_reader::SDAT_CHUNK_ID });
		if (sdat_offset == -1)
			return false;

		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, sdat_offset + header_size));

		return true;
	}
	bool DigiTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk)
	{
		// Get HSHD chunk for the sample rate.
		size_t hshd_offset = getOffsetChunk(fc, offset, { chunk_reader::HSHD_CHUNK_ID });
		if (hshd_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		size_t sdat_offset = getOffsetChunk(fc, offset, { chunk_reader::SDAT_CHUNK_ID });
		if (sdat_offset == -1)
			return false;

		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, sdat_offset + header_size));

		return true;
	}
	bool RoomBackgroundTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, img_info& info)
	{
		size_t type_offset = getOffsetChunk(fc, offset, {
			chunk_reader::BMAP_CHUNK_ID,
			chunk_reader::SMAP_CHUNK_ID,
			});
		if (type_offset == -1)
			return false;

		chunk_reader::ChunkInfo typeChunk = fc->GetChunkInfo(type_offset);
		if (utils::chunkcmp(typeChunk.chunk_id, chunk_reader::BMAP_CHUNK_ID) == 0)
			return ImageTab::DecodeBMAP(fc, offset, info);
		else if (utils::chunkcmp(typeChunk.chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
		{

		}

		return false;
	}
	//bool RoomImageTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, std::vector<img_info>& info)
	//{
	//	img_info inf;
	//	ImageTab::DecodeSMAP(fc, offset, inf);
	//	info.push_back(inf);
	//	return info.size() > 0;
	//}
	bool ImageTab::DecodeBMAP(chunk_reader::FileContainer*& fc, size_t offset, img_info& info)
	{
		// Get RMHD chunk for width and height of image.
		size_t rmhd_offset = getOffsetChunk(fc, offset, { chunk_reader::RMHD_CHUNK_ID });
		if (rmhd_offset == -1)
			return false;

		chunk_reader::RMHD_Chunk rmhd_chunk;
		memcpy(&rmhd_chunk, utils::add(fc->data, rmhd_offset), sizeof(chunk_reader::RMHD_Chunk));

		// Get BMAP chunk for raw data.
		size_t bmap_offset = getOffsetChunk(fc, offset, { chunk_reader::BMAP_CHUNK_ID });
		if (bmap_offset == -1)
			return false;

		chunk_reader::BMAP_Chunk bmap_chunk;
		size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
		memcpy(&bmap_chunk, utils::add(fc->data, bmap_offset), header_size);
		bmap_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, bmap_offset + header_size));
		size_t bmap_size = bmap_chunk.ChunkSize() - header_size;

		// Get TRNS chunk for transparency settings.
		size_t trns_offset = getOffsetChunk(fc, offset, { chunk_reader::TRNS_CHUNK_ID });
		if (trns_offset == -1)
			return false;

		chunk_reader::TRNS_Chunk trns_chunk;
		memcpy(&trns_chunk, utils::add(fc->data, trns_offset), sizeof(chunk_reader::TRNS_Chunk));

		size_t apal_offset = getOffsetChunk(fc, offset, { chunk_reader::APAL_CHUNK_ID });
		if (apal_offset == -1)
			return false;

		chunk_reader::APAL_Chunk apal_chunk;
		header_size = sizeof(chunk_reader::APAL_Chunk) - sizeof(apal_chunk.data);
		memcpy(&apal_chunk, utils::add(fc->data, apal_offset), header_size);
		size_t apal_size = apal_chunk.ChunkSize() - header_size;
		apal_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, apal_offset + header_size));

		int palen = bmap_chunk.encoding % 10;

		bool he = bmap_chunk.encoding >= 0x86 && bmap_chunk.encoding <= 0x8A;
		bool he_transparent = bmap_chunk.encoding >= 0x90 && bmap_chunk.encoding <= 0x94;

		if (!DecodeHE(bmap_chunk.fill_color, bmap_chunk.data, bmap_size, rmhd_chunk.width, rmhd_chunk.height, palen, he_transparent, info))
			return false;

		std::vector<uint8_t> newOut;
		for (size_t i = 0; i < info.size; i++)
		{
			newOut.push_back(apal_chunk.data[info.data[i] * 3]);
			newOut.push_back(apal_chunk.data[info.data[i] * 3 + 1]);
			newOut.push_back(apal_chunk.data[info.data[i] * 3 + 2]);
			if (he_transparent)
			{
				if (info.data[i] == bmap_chunk.fill_color)
					newOut.push_back(0);
				else
					newOut.push_back(255);
			}
		}

		free(info.data);

		info.size = newOut.size();
		info.data = reinterpret_cast<unsigned char*>(malloc(newOut.size()));
		memcpy(info.data, newOut.data(), newOut.size());

		return true;
	}
	//struct offset_pair
	//{
	//	size_t start, end;
	//};
	//struct strip
	//{
	//	unsigned char* data;
	//	size_t size;
	//};
	//bool ImageTab::DecodeSMAP(chunk_reader::FileContainer*& fc, size_t offset, img_info& info)
	//{
	//	// Get IMHD chunk for width and height of image.
	//	size_t imhd_offset = getOffsetChunk(fc, offset, { chunk_reader::IMHD_CHUNK_ID });
	//	if (imhd_offset == -1)
	//		return false;

	//	chunk_reader::IMHD_Chunk imhd_chunk;
	//	memcpy(&imhd_chunk, utils::add(fc->data, imhd_offset), sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data));
	//	imhd_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, imhd_offset + (sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data))));

	//	// Get SMAP chunk for raw data.
	//	size_t smap_offset = getOffsetChunk(fc, offset, { chunk_reader::SMAP_CHUNK_ID });
	//	if (smap_offset == -1)
	//		return false;

	//	chunk_reader::SMAP_Chunk smap_chunk;
	//	size_t header_size = sizeof(chunk_reader::SMAP_Chunk) - sizeof(smap_chunk.data); // Pointer in the SMAP class is size 8 and needs to be deducted.
	//	memcpy(&smap_chunk, utils::add(fc->data, smap_offset), header_size);
	//	smap_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, smap_offset + header_size));
	//	size_t smap_size = smap_chunk.ChunkSize() - header_size;

	//	int magic_number = 8;
	//	uint32_t strip_width = magic_number;

	//	size_t num_strips = static_cast<size_t>(floor(imhd_chunk.width / strip_width));

	//	std::vector<uint32_t> offsets;
	//	int j = 0;
	//	for (size_t i = 0; i < num_strips; i++, j += sizeof(uint32_t))
	//	{
	//		uint32_t number = *reinterpret_cast<uint32_t*>(utils::add(smap_chunk.data, j));
	//		number -= magic_number;
	//		offsets.push_back(number);
	//	}

	//	std::vector<offset_pair> index;

	//	for (size_t i = 0; i < offsets.size(); i++)
	//		index.push_back({offsets[i], (i + 1) == offsets.size() ? smap_size : offsets[i + 1]});

	//	std::vector<strip> strips;
	//	for (size_t i = 0; i < num_strips; i++)
	//		strips.push_back({ reinterpret_cast<unsigned char*>(utils::add(smap_chunk.data, index[i].start)), index[i].end });

	//	size_t total_size = 0;
	//	std::vector<img_info> data_blocks;
	//	for (size_t i = 0; i < strips.size(); i++)
	//	{
	//		uint8_t code = strips[i].data[0];

	//		bool horizontal = true;
	//		if (code >= 0x03 && code <= 0x12 || code >= 0x22 && code <= 0x26)
	//			horizontal = false;

	//		bool he = code >= 0x86 && code <= 0x94;
	//		bool he_transparent = code >= 0x22 && code <= 0x30 || code >= 0x54 && code <= 0x80 || code >= 0x8F;
	//		// Between 134 and 138 (134 and 138 counted)
	//		// Between 144 and 148 (144 and 148 counted) means it is transparent as well.
	//		if (he || he_transparent)
	//		{
	//			uint8_t transparency = 0;
	//			if (he_transparent)
	//				transparency = 255;

	//			int palen = code % 10;

	//			img_info strip_info;
	//			if (!DecodeBasic(strips[i].data[1], reinterpret_cast<unsigned char*>(utils::add(strips[i].data, 2)), strips[i].size - 2, strip_width, imhd_chunk.height, palen, he_transparent, info))
	//				return false;


	//			// Vertical.
	//			unsigned char* data = reinterpret_cast<unsigned char*>(malloc(info.size));
	//			if (horizontal)
	//			{

	//			}
	//			else
	//			{
	//				int dataIndex = 0; // Index to traverse the 1D data
	//				for (int i = 0; i < strip_width; ++i)
	//				{
	//					for (int j = 0; j < imhd_chunk.height; ++j)
	//					{
	//						data[(j * strip_width) + i] = data[dataIndex];
	//						++dataIndex;
	//					}
	//				}
	//			}

	//			free(info.data);
	//			info.data = data;
	//			total_size += info.size;
	//			data_blocks.push_back(info);
	//		}
	//	}

	//	unsigned char* total_data = reinterpret_cast<unsigned char*>(malloc(total_size));
	//	size_t prev_offset = 0;
	//	for (size_t i = 0; i < data_blocks.size(); i++)
	//	{
	//		memcpy(reinterpret_cast<unsigned char*>(utils::add(total_data, prev_offset)), data_blocks[i].data, data_blocks[i].size);
	//		free(data_blocks[i].data);
	//		prev_offset += data_blocks[i].size;
	//	}

	//	info.data = total_data;
	//	info.size = total_size;
	//	return true;
	//}
	bool ImageTab::DecodeHE(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info)
	{
		info.width = width;
		info.height = height;

		std::vector<int> delta_color = { -4, -3, -2, -1, 1, 2, 3, 4 };

		unsigned char color = fill_color;

		std::vector<uint8_t> bits = create_bitstream(data, data_size);

		std::vector<uint8_t> out;

		info.channels = 3;
		if (transparent)
			info.channels = 4;

		out.push_back(color % 256);

		size_t num_pixels = info.width * info.height;

		int pos = 0;
		while (out.size() < num_pixels)
		{
			if (bits[pos++] == 1)
			{
				if (bits[pos++] == 1)
				{
					uint8_t bitc = collect_bits(pos, bits, 3);
					color += delta_color[bitc];
				}
				else
				{
					color = collect_bits(pos, bits, palen);
				}
			}
			out.push_back(color % 256);
		};

		info.size = out.size();
		info.data = reinterpret_cast<unsigned char*>(malloc(out.size()));
		memcpy(info.data, out.data(), out.size());
		return true;
	}
	bool ImageTab::DecodeBasic(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info)
	{
		info.width = width;
		info.height = height;

		unsigned char color = fill_color;

		std::vector<uint8_t> bits = create_bitstream(data, data_size);

		std::vector<uint8_t> out;

		info.channels = 3;
		if (transparent)
			info.channels = 4;

		out.push_back(color % 256);

		size_t num_pixels = info.width * info.height;

		int sub = 1;
		int pos = 0;
		while (out.size() < num_pixels)
		{
			if (bits[pos++] == 1)
			{
				if (bits[pos++] == 1)
				{
					if (bits[pos++] == 1)
						sub = -sub;
					color -= sub;
				}
				else
				{
					color = collect_bits(pos, bits, palen);
					sub = 1;
				}
			}
			out.push_back(color % 256);
		};

		info.size = out.size();
		info.data = reinterpret_cast<unsigned char*>(malloc(out.size()));
		memcpy(info.data, out.data(), out.size());
		return true;
	}
}