#include "functions/TabFunctions.h"

#include "lowlevel/HumongousChunks.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include "file/Files.h"
#include "forms/HumongousButton.h"
#include "systems/AudioSystem.h"
#include "file/BMPHeader.h"
#include "file/BMPTransparency.h"
#include <bitset>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>

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
	int32_t getOffsetChunk(chunk_reader::FileContainer*& fc, size_t offset, const char* chunk_name)
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
			System::Windows::Forms::MessageBox::Show("Cannot process file type", "Decompilation failed", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
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
		audioSystem.Stop();
	}
	void TabFunctions::AddTab(HumongousNode^ node, System::Windows::Forms::TabControl^ tabControl)
	{
		chunk_reader::FileContainer* fc = files::FILES.getFile(node->fileType);

		if (fc == nullptr)
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
		AddInfoRow("Name", node->Text, propertyGrid, posX, posY);
		AddInfoRow("Pos", gcnew System::String(std::to_string(node->offset).c_str()), propertyGrid, posX, posY);

		chunk_reader::ChunkInfo chunk = fc->GetChunkInfo(node->offset);
		if (utils::chunkcmp(chunk.chunk_id, chunk_reader::SGEN_CHUNK_ID) == 0)
			GetSong(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0)
			GetTalk(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::DIGI_CHUNK_ID) == 0)
			GetDigi(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::SCRP_CHUNK_ID) == 0)
			GetScrp(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
		else if (
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM00_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM01_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM02_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM03_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM04_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM05_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM06_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM07_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM08_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM09_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM10_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM11_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM12_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM13_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM14_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM15_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM16_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM17_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM0A_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM0B_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM0C_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM0D_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM0E_CHUNK_ID) == 0 ||
				utils::chunkcmp(chunk.chunk_id, chunk_reader::IM0F_CHUNK_ID) == 0)
			GetImXX(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::DISK_CHUNK_ID) == 0)
			GetRNAM(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);

		// Construct export button.
		HumongousButton^ exportButton;
		exportButton = (gcnew HumongousButton());

		exportButton->Location = System::Drawing::Point(232, 53);
		exportButton->Name = gcnew System::String("Export_") + gcnew System::String(newTab->Name);
		exportButton->Size = System::Drawing::Size(75, 23);
		exportButton->TabIndex = 2;
		exportButton->offset = node->offset;
		exportButton->Text = L"Export";
		exportButton->UseVisualStyleBackColor = true;

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

		//System::Windows::Forms::TextBox^ textBox1 = gcnew System::Windows::Forms::TextBox();
		//textBox1->Location = System::Drawing::Point(panel->Location.X, panel->Location.Y);
		//textBox1->Name = L"textBox1";
		//textBox1->Size = System::Drawing::Size(panel->Size.Width, panel->Size.Height);
		//textBox1->TabIndex = 2;

		//chunk_reader::SCRP_Chunk scrp_chunk;
		//memcpy(&scrp_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::SCRP_Chunk) - sizeof(scrp_chunk.data));
		//scrp_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, offset + sizeof(chunk_reader::HumongousHeader)));

		//std::string t = std::string(reinterpret_cast<char*>(scrp_chunk.data));
		//t.resize(scrp_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader));
		//textBox1->Text = gcnew System::String(t.c_str());

		//tab->Controls->Add(textBox1);
	}
	void TabFunctions::GetImXX(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		//AddInfoRow("Type", gcnew System::String("Image"), propertyGrid, posX, posY);

		//// Get RMHD chunk for width and height of image.
		//size_t rmhd_offset = getOffsetChunk(fc, offset, chunk_reader::RMHD_CHUNK_ID);
		//chunk_reader::RMHD_Chunk rmhd_chunk;
		//memcpy(&rmhd_chunk, utils::add(fc->data, rmhd_offset), sizeof(chunk_reader::RMHD_Chunk));

		//AddInfoRow("Width", gcnew System::String(std::to_string(rmhd_chunk.width).c_str()), propertyGrid, posX, posY);
		//AddInfoRow("Height", gcnew System::String(std::to_string(rmhd_chunk.height).c_str()), propertyGrid, posX, posY);

		//// Get BMAP chunk for raw data.
		//size_t bmap_offset = getOffsetChunk(fc, offset, chunk_reader::BMAP_CHUNK_ID);
		//chunk_reader::BMAP_Chunk bmap_chunk;
		//size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
		//memcpy(&bmap_chunk, utils::add(fc->data, bmap_offset), header_size);
		//bmap_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, bmap_offset + header_size));

		//// Get TRNS chunk for transparency settings.
		//size_t trns_offset = getOffsetChunk(fc, offset, chunk_reader::TRNS_CHUNK_ID);
		//chunk_reader::TRNS_Chunk trns_chunk;
		//memcpy(&trns_chunk, utils::add(fc->data, trns_offset), sizeof(chunk_reader::TRNS_Chunk));

		//// Actual bmap byte size.
		//size_t bmap_size = bmap_chunk.ChunkSize() - header_size;

		//BMAP::BMPTransparency transparency;
		//if (bmap_chunk.transparency >= 134 && bmap_chunk.transparency <= 138)
		//	transparency = BMAP::BMPTransparency::Not_Transparent;
		//else if (bmap_chunk.transparency >= 144 && bmap_chunk.transparency <= 148)
		//	transparency = BMAP::BMPTransparency::Transparent;
		//else if (bmap_chunk.transparency == 1 || bmap_chunk.transparency == 149)
		//	transparency = BMAP::BMPTransparency::NotCompressed;
		//else if (bmap_chunk.transparency == 150)
		//	transparency = BMAP::BMPTransparency::NoIdea;
		//else
		//	transparency = BMAP::BMPTransparency::Invalid;

		//AddInfoRow("Transparent", gcnew System::String(transparency == BMAP::BMPTransparency::Transparent ? "Yes" : "No"), propertyGrid, posX, posY);

		////std::string bits;
		////for (size_t i = 0; i < bmap_size; i++)
		////	bits += std::bitset<8>(reinterpret_cast<unsigned char*>(utils::add(bmap_chunk.data, i))).to_string();

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

		AddInfoRow("Number of Rooms", gcnew System::String(std::to_string(disk_chunk.num_rooms).c_str()), propertyGrid, posX, posY);

		// Get HSHD chunk for the sample rate.
		int32_t rnam_offset = getOffsetChunk(fc, offset, chunk_reader::RNAM_CHUNK_ID);
		if (rnam_offset == -1)
			return;

		chunk_reader::RNAM_Chunk* rnam_chunk = reinterpret_cast<chunk_reader::RNAM_Chunk*>(utils::add(fc->data, rnam_offset));

		std::vector<std::string> room_names;

		size_t rnam_end = rnam_offset + rnam_chunk->ChunkSize();
		size_t pos = rnam_offset + sizeof(chunk_reader::HumongousHeader) + sizeof(uint16_t);
		std::string room_name;

		while (pos < rnam_end)
		{
			unsigned char ch;
			memcpy(&ch, utils::add(fc->data, pos), sizeof(char));
			if (utils::unsignedCharCmp(ch, '\0'))
			{
				room_names.push_back(room_name);
				AddInfoRow(gcnew System::String(std::to_string(room_names.size()).c_str()), gcnew System::String(room_name.c_str()), propertyGrid, posX, posY);
				room_name = "";
				pos += sizeof(uint16_t);
			}
			else
				room_name += ch;
			pos++;
		}
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
		int32_t hshd_offset = getOffsetChunk(fc, offset, chunk_reader::HSHD_CHUNK_ID);
		if (hshd_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		int32_t sdat_offset = getOffsetChunk(fc, offset, chunk_reader::SDAT_CHUNK_ID);
		if (sdat_offset == -1)
			return false;

		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, sdat_offset + header_size));
		
		return true;
	}

	bool SongTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk)
	{
		// Get SGEN chunk first (tells us the position of the SONG.
		chunk_reader::SGEN_Chunk sgen_chunk;
		memcpy(&sgen_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::SGEN_Chunk));

		// Get HSHD chunk for the sample rate.
		int32_t hshd_offset = getOffsetChunk(fc, sgen_chunk.song_pos, chunk_reader::HSHD_CHUNK_ID);
		if (hshd_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		int32_t sdat_offset = getOffsetChunk(fc, sgen_chunk.song_pos, chunk_reader::SDAT_CHUNK_ID);
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
		int32_t hshd_offset = getOffsetChunk(fc, offset, chunk_reader::HSHD_CHUNK_ID);
		if (hshd_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		int32_t sdat_offset = getOffsetChunk(fc, offset, chunk_reader::SDAT_CHUNK_ID);
		if (sdat_offset == -1)
			return false;

		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, sdat_offset + header_size));

		return true;
	}
}