#include "functions/TabFunctions.h"

#include <bitset>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <windows.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "lowlevel/HumongousChunks.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include "file/Files.h"
#include "forms/HumongousButton.h"
#include "systems/AudioSystem.h"
#include "file/BMPHeader.h"
#include "file/BMPTransparency.h"
#include "forms/HumongousNode.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "systems/Logger.h"
#include "file/ResourceType.h"

// Fixed decryption error when indexing .(a) and HE0 files
// Added export functionality to wav files.
// Added RNAM chunk to info tabs.

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
	size_t getOffsetChunk(chunk_reader::FileContainer*& fc, size_t offset, const char* chunk_name)
	{
		chunk_reader::ChunkInfo info = fc->GetChunkInfo(offset);

		int i = 0;
		while (i < LOOKUP_LIMIT && offset < fc->size)
		{
			if (utils::chunkcmp(info.chunk_id, chunk_name) == 0)
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
						\0*.WAV;*.wav\0";;
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
			case files::ResourceType::Image:
			{
				break;
			}
			default:
			{
				break;
			}
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

		System::Windows::Forms::DataGridView^ propertyGrid = (gcnew System::Windows::Forms::DataGridView());
		newTab->Controls->Add(propertyGrid);
		propertyGrid->Dock = System::Windows::Forms::DockStyle::Fill;
		propertyGrid->Location = System::Drawing::Point(0, 0);
		propertyGrid->Name = L"propertyGrid1";
		propertyGrid->Size = System::Drawing::Size(497, 462);
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
			case files::ResourceType::Image:
			{
				GetImXX(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
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
	void TabFunctions::GetImXX(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Image"), propertyGrid, posX, posY);

		// Get RMHD chunk for width and height of image.
		size_t rmhd_offset = getOffsetChunk(fc, offset, chunk_reader::RMHD_CHUNK_ID);
		chunk_reader::RMHD_Chunk rmhd_chunk;
		memcpy(&rmhd_chunk, utils::add(fc->data, rmhd_offset), sizeof(chunk_reader::RMHD_Chunk));
		unsigned char* d = reinterpret_cast<unsigned char*>(utils::add(fc->data, rmhd_offset));

		AddInfoRow("Width", gcnew System::String(std::to_string(rmhd_chunk.width).c_str()), propertyGrid, posX, posY);
		AddInfoRow("Height", gcnew System::String(std::to_string(rmhd_chunk.height).c_str()), propertyGrid, posX, posY);

		// Get BMAP chunk for raw data.
		size_t bmap_offset = getOffsetChunk(fc, offset, chunk_reader::BMAP_CHUNK_ID);
		chunk_reader::BMAP_Chunk bmap_chunk;
		size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
		memcpy(&bmap_chunk, utils::add(fc->data, bmap_offset), header_size);
		size_t bmap_size = bmap_chunk.ChunkSize() - header_size - sizeof(bmap_chunk.transparency) - sizeof(bmap_chunk.fill_color);
		bmap_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, bmap_offset + header_size + sizeof(bmap_chunk.transparency) + sizeof(bmap_chunk.fill_color)));

		// Get TRNS chunk for transparency settings.
		size_t trns_offset = getOffsetChunk(fc, offset, chunk_reader::TRNS_CHUNK_ID);
		chunk_reader::TRNS_Chunk trns_chunk;
		memcpy(&trns_chunk, utils::add(fc->data, trns_offset), sizeof(chunk_reader::TRNS_Chunk));

		// Actual bmap byte size.

		BMAP::BMPTransparency transparency;
		if (bmap_chunk.transparency >= 134 && bmap_chunk.transparency <= 138)
			transparency = BMAP::BMPTransparency::Not_Transparent;
		else if (bmap_chunk.transparency >= 144 && bmap_chunk.transparency <= 148)
			transparency = BMAP::BMPTransparency::Transparent;
		else if (bmap_chunk.transparency == 1 || bmap_chunk.transparency == 149)
			transparency = BMAP::BMPTransparency::NotCompressed;
		else if (bmap_chunk.transparency == 150)
			transparency = BMAP::BMPTransparency::NoIdea;
		else
			transparency = BMAP::BMPTransparency::Invalid;

		AddInfoRow("Transparent", gcnew System::String(transparency == BMAP::BMPTransparency::Transparent ? "Yes" : "No"), propertyGrid, posX, posY);

		size_t num_pixels = rmhd_chunk.width * rmhd_chunk.height;

		std::string bits;
		for (size_t i = 0; i < bmap_size; i++)
		{
			unsigned char* pC = reinterpret_cast<unsigned char*>(utils::add(bmap_chunk.data, i));
			char c = *pC;
			bits += c >> 0;
		}

		printf("Test\n ");
		////auto gbits = grouper<std::string>(bits, 1);
		////std::vector<int> bmap;
		////bmap.reserve(rmhd_chunk.height * rmhd_chunk.width);
		////for (int i = 0; i < rmhd_chunk.height * rmhd_chunk.width; ++i) {
		////	std::string bit_str;
		////	std::copy_n(std::prev(gbits[i / bpp].base()), bpp,
		////		std::back_inserter(bit_str));
		////	bmap.push_back(std::stoi(bit_str, 0, 2));
		////}

		//FILE* rfile = nullptr;

		//// Open the file.
		//fopen_s(&rfile, "D:/ekkes/download.bmp", "rb");
		//if (rfile == nullptr)
		//{
		//	return;
		//}

		//BMAP::BMPHead bmpHead;
		//BMAP::BMPHeader header;

		//int infohd_size = sizeof(BMAP::BMPHeader);
		//int offbits = sizeof(BMAP::BMPHead) + infohd_size + 1024;
		//int bitcount = 8;
		//int sizeimage = total_bmap_pixels_size;
		//int fsize = offbits + sizeimage;
		//int xpels = 0;
		//int ypels = 0;
		//int clrused = 256;
		//int clrimp = 256;

		//header.infohd_width = rmhd_chunk.width;
		//header.infohd_width = rmhd_chunk.height;
		//bmpHead.filehd_size = fsize;
		//bmpHead.filehd_offbits = offbits;
		//header.infohd_size = infohd_size;
		//header.infohd_bitcount = bitcount;
		//header.infohd_sizeimage = sizeimage;
		//header.infohd_xpelsm = xpels;
		//header.infohd_ypelsm = ypels;
		//header.infohd_clrused = clrused;
		//header.infohd_clrimp = clrimp;

		//fwrite(&bmpHead, sizeof(bmpHead), 1, rfile);
		//fwrite(&header, sizeof(header), 1, rfile);

		//System::IO::MemoryStream^ ms = gcnew System::IO::MemoryStream(ar);
		//System::Drawing::Image^ pic = Image::FromStream(ms);

		//pictureBox->Dock = System::Windows::Forms::DockStyle::Bottom;
		//pictureBox->Location = System::Drawing::Point(3, 306);
		//pictureBox->Name = L"Action Panel";
		//float d = 1.0f / pic->Width * tab->Width;
		//pictureBox->Size = System::Drawing::Size(tab->Width, pic->Height * d);
		//pictureBox->Image = pic;
		//pictureBox->SizeMode = PictureBoxSizeMode::StretchImage;

		//tab->Controls->Add(pictureBox);
	}
	void TabFunctions::GetRNAM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Rooms"), propertyGrid, posX, posY);

		chunk_reader::DISK_Chunk disk_chunk;
		memcpy(&disk_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::DISK_Chunk) - sizeof(disk_chunk.data));

		// Get RNAM chunk for the sample rate.
		size_t rnam_offset = getOffsetChunk(fc, offset, chunk_reader::RNAM_CHUNK_ID);
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
		size_t hshd_offset = getOffsetChunk(fc, offset, chunk_reader::HSHD_CHUNK_ID);
		if (hshd_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		size_t sdat_offset = getOffsetChunk(fc, offset, chunk_reader::SDAT_CHUNK_ID);
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
		size_t hshd_offset = getOffsetChunk(fc, sgen_chunk.song_pos, chunk_reader::HSHD_CHUNK_ID);
		if (hshd_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		size_t sdat_offset = getOffsetChunk(fc, sgen_chunk.song_pos, chunk_reader::SDAT_CHUNK_ID);
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
		size_t hshd_offset = getOffsetChunk(fc, offset, chunk_reader::HSHD_CHUNK_ID);
		if (hshd_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		size_t sdat_offset = getOffsetChunk(fc, offset, chunk_reader::SDAT_CHUNK_ID);
		if (sdat_offset == -1)
			return false;

		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, sdat_offset + header_size));

		return true;
	}
}