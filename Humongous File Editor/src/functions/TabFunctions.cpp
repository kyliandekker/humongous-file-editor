#include "functions/TabFunctions.h"

#include "lowlevel/HumongousChunks.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include "file/Files.h"
#include "forms/HumongousButton.h"
#include "systems/AudioSystem.h"
#include "file/BMPHeader.h"
#include <bitset>

namespace HumongousFileEditor
{
	size_t getOffsetChunk(chunk_reader::FileContainer*& fc, size_t offset, const char* chunk_name)
	{
		chunk_reader::ChunkInfo info = fc->GetChunkInfo(offset);

		int i = 0;
		while (i < 15 && offset < fc->size)
		{
			if (utils::chunkcmp(info.chunk_id, chunk_name) == 0)
				break;
			info = fc->GetNextChunk(info.offset);
		}
		if (i >= 15)
			return -1;

		return info.offset;
	}
	// Adds a row of info to the info tab.
	void AddInfoRow(System::String^ key, System::String^ value, System::Windows::Forms::TabPage^ tab, float& posX, float& posY)
	{
		System::Windows::Forms::Label^ keyLabel;
		keyLabel = (gcnew System::Windows::Forms::Label());
		keyLabel->SuspendLayout();

		System::Windows::Forms::Label^ valueLabel;
		valueLabel = (gcnew System::Windows::Forms::Label());
		valueLabel->SuspendLayout();

		keyLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
			| System::Windows::Forms::AnchorStyles::Right));
		keyLabel->AutoSize = true;
		keyLabel->Location = System::Drawing::Point(posX, posY);
		keyLabel->Name = key;
		keyLabel->Size = System::Drawing::Size(35, 13);
		keyLabel->TabIndex = 0;
		keyLabel->Text = key;
		keyLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;

		valueLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
			| System::Windows::Forms::AnchorStyles::Right));
		valueLabel->AutoSize = true;
		valueLabel->Location = System::Drawing::Point(posX + 100, posY);
		valueLabel->Name = value;
		valueLabel->Size = System::Drawing::Size(35, 13);
		valueLabel->TabIndex = 1;
		valueLabel->Text = value;
		valueLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;

		tab->Controls->Add(keyLabel);
		tab->Controls->Add(valueLabel);

		posY += 40;
	}

	// Callback for the play button.
	System::Void TabFunctions::PlayButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		HumongousButton^ btn = (HumongousButton^)sender;

		chunk_reader::FileContainer* fc = nullptr;
		switch (btn->fileType)
		{
			case files::FileType_A:
			{
				fc = files::FILES.a;
				break;
			}
			case files::FileType_HE2:
			{
				fc = files::FILES.he2;
				break;
			}
			case files::FileType_HE4:
			{
				fc = files::FILES.he4;
				break;
			}
		}

		if (fc == nullptr)
			return;

		chunk_reader::SDAT_Chunk sdat_chunk;
		memcpy(&sdat_chunk, utils::add(fc->data, btn->offset), sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data));
		sdat_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, btn->offset + sizeof(chunk_reader::HumongousHeader)));

		audioSystem.Stop();
		audioSystem.Play(reinterpret_cast<unsigned char*>(utils::add(fc->data, btn->offset + sizeof(chunk_reader::HumongousHeader))), sdat_chunk.ChunkSize());
	}
	// Callback for the stop button.
	System::Void TabFunctions::StopButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		audioSystem.Stop();
	}
	void TabFunctions::AddTab(HumongousNode^ node, System::Windows::Forms::TabControl^ tabControl)
	{
		chunk_reader::FileContainer* fc = nullptr;
		switch (node->fileType)
		{
			case files::FileType_A:
			{
				fc = files::FILES.a;
				break;
			}
			case files::FileType_HE2:
			{
				fc = files::FILES.he2;
				break;
			}
			case files::FileType_HE4:
			{
				fc = files::FILES.he4;
				break;
			}
		}

		if (fc == nullptr)
			return;

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

		System::Windows::Forms::Panel^ actionPanel;
		actionPanel = (gcnew System::Windows::Forms::TableLayoutPanel());

		actionPanel->Dock = System::Windows::Forms::DockStyle::Bottom;
		actionPanel->Location = System::Drawing::Point(3, 306);
		actionPanel->Name = L"Action Panel";
		actionPanel->Size = System::Drawing::Size(418, 87);
		actionPanel->TabIndex = 1;

		float posX = 35, posY = 35;
		AddInfoRow("Name", node->Text, newTab, posX, posY);
		AddInfoRow("Pos", gcnew System::String(std::to_string(node->offset).c_str()), newTab, posX, posY);

		chunk_reader::ChunkInfo chunk = fc->GetChunkInfo(node->offset);
		if (utils::chunkcmp(chunk.chunk_id, chunk_reader::SGEN_CHUNK_ID) == 0)
			GetSong(fc, node->offset, newTab, actionPanel, posX, posY);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0)
			GetTalk(fc, node->offset, newTab, actionPanel, posX, posY);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::DIGI_CHUNK_ID) == 0)
			GetDigi(fc, node->offset, newTab, actionPanel, posX, posY);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::SCRP_CHUNK_ID) == 0)
			GetScrp(fc, node->offset, newTab, actionPanel, posX, posY);
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
			GetImXX(fc, node->offset, newTab, actionPanel, posX, posY);

		HumongousButton^ exportButton;
		exportButton = (gcnew HumongousButton());

		exportButton->Location = System::Drawing::Point(232, 53);
		exportButton->Name = gcnew System::String("Export_") + gcnew System::String(newTab->Name);
		exportButton->Size = System::Drawing::Size(75, 23);
		exportButton->TabIndex = 2;
		exportButton->Text = L"Export";
		exportButton->UseVisualStyleBackColor = true;

		exportButton->ResumeLayout(false);
		actionPanel->Controls->Add(exportButton);

		newTab->ResumeLayout(false);

		newTab->Controls->Add(actionPanel);
		tabControl->Controls->Add(newTab);
	}
	void TabFunctions::GetTalk(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Talk"), tab, posX, posY);

		// Get HSHD chunk for the sample rate.
		size_t hshd_offset = getOffsetChunk(fc, offset, chunk_reader::HSHD_CHUNK_ID);
		chunk_reader::HSHD_Chunk hshd_chunk;
		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		size_t sdat_offset = getOffsetChunk(fc, offset, chunk_reader::SDAT_CHUNK_ID);
		chunk_reader::SDAT_Chunk sdat_chunk;
		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, sdat_offset + header_size));

		AddInfoRow("Sample Rate", gcnew System::String(std::to_string(hshd_chunk.sample_rate).c_str()), tab, posX, posY);
		AddInfoRow("Size (in bytes)", gcnew System::String(std::to_string(sdat_chunk.ChunkSize()).c_str()), tab, posX, posY);

		AddSoundButtons(tab, sdat_offset, fc->fileType, panel);
	}
	void TabFunctions::GetDigi(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Sfx"), tab, posX, posY);

		// Get HSHD chunk for the sample rate.
		size_t hshd_offset = getOffsetChunk(fc, offset, chunk_reader::HSHD_CHUNK_ID);
		chunk_reader::HSHD_Chunk hshd_chunk;
		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		size_t sdat_offset = getOffsetChunk(fc, offset, chunk_reader::SDAT_CHUNK_ID);
		chunk_reader::SDAT_Chunk sdat_chunk;
		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, sdat_offset + header_size));

		AddInfoRow("Sample Rate", gcnew System::String(std::to_string(hshd_chunk.sample_rate).c_str()), tab, posX, posY);
		AddInfoRow("Size (in bytes)", gcnew System::String(std::to_string(sdat_chunk.ChunkSize()).c_str()), tab, posX, posY);

		AddSoundButtons(tab, sdat_offset, fc->fileType, panel);
	}
	void TabFunctions::GetSong(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Song"), tab, posX, posY);

		// Get SGEN chunk first (tells us the position of the SONG.
		chunk_reader::SGEN_Chunk sgen_chunk;
		memcpy(&sgen_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::SGEN_Chunk));

		// Get HSHD chunk for the sample rate.
		size_t hshd_offset = getOffsetChunk(fc, sgen_chunk.song_pos, chunk_reader::HSHD_CHUNK_ID);
		chunk_reader::HSHD_Chunk hshd_chunk;
		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		size_t sdat_offset = getOffsetChunk(fc, sgen_chunk.song_pos, chunk_reader::SDAT_CHUNK_ID);
		chunk_reader::SDAT_Chunk sdat_chunk;
		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, sdat_offset + header_size));

		AddInfoRow("Sample Rate", gcnew System::String(std::to_string(hshd_chunk.sample_rate).c_str()), tab, posX, posY);
		AddInfoRow("Size (in bytes)", gcnew System::String(std::to_string(sdat_chunk.ChunkSize()).c_str()), tab, posX, posY);

		AddSoundButtons(tab, sdat_offset, fc->fileType, panel);
	}
	void TabFunctions::GetScrp(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Script"), tab, posX, posY);

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
	//void rzil()
	//{
	//	std::string bits;
	//	for (size_t i = 0; i < bmap_size; i++)
	//		bits += std::bitset<8>(reinterpret_cast<unsigned char>(utils::add(bmap_chunk.data, i))).to_string();
	//}
	void TabFunctions::GetImXX(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Image"), tab, posX, posY);

		// Get RMHD chunk for width and height of image.
		size_t rmhd_offset = getOffsetChunk(fc, offset, chunk_reader::RMDA_CHUNK_ID);
		chunk_reader::RMHD_Chunk rmhd_chunk;
		memcpy(&rmhd_chunk, utils::add(fc->data, rmhd_offset), sizeof(chunk_reader::RMHD_Chunk));

		AddInfoRow("Width", gcnew System::String(std::to_string(rmhd_chunk.width).c_str()), tab, posX, posY);
		AddInfoRow("Height", gcnew System::String(std::to_string(rmhd_chunk.height).c_str()), tab, posX, posY);

		// Get BMAP chunk for raw data.
		size_t bmap_offset = getOffsetChunk(fc, offset, chunk_reader::BMAP_CHUNK_ID);
		chunk_reader::BMAP_Chunk bmap_chunk;
		size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
		memcpy(&bmap_chunk, utils::add(fc->data, bmap_offset), header_size);
		bmap_chunk.data = reinterpret_cast<unsigned char*>(utils::add(fc->data, bmap_offset + header_size));

		// Get TRNS chunk for transparency settings.
		size_t trns_offset = getOffsetChunk(fc, offset, chunk_reader::RMDA_CHUNK_ID);
		chunk_reader::TRNS_Chunk trns_chunk;
		memcpy(&trns_chunk, utils::add(fc->data, trns_offset), sizeof(chunk_reader::TRNS_Chunk));

		// Actual bmap byte size.
		size_t bmap_size = bmap_chunk.ChunkSize() - header_size;

		uint8_t encoding = bmap_chunk.transparency;

		bool rle = false;			// uses RLE encoding?
		bool horiz = false;			// draws horizontal or vertical?
		bool trans = false;			// has transparency?
		bool exprange = false;		// expanded range for 3-bit relative palette set?
									// ([-4, -1] and [1, 4] instead of [-4, 3])
		uint8_t bpabsol = 0;			// bits per absolute palette set
		uint8_t bprel = 0;				// bits per relative palette set

		if (encoding >= 134 && encoding <= 138)
		{
			// Not transparent.
		}
		else if (encoding >= 144 && encoding <= 148)
		{
			// Transparent.
		}
		else if (encoding == 1 || encoding == 149)
		{
			// Not compressed, not RLE.
		}
		else if (encoding == 150)
		{
			// No idea.
		}
		else
		{
			// Invalid.
		}

		bpabsol = encoding % 10;

		if (encoding <= 48)
			bprel = 1;
		else
			bprel = 3;

		size_t total_bmap_pixels_size = rmhd_chunk.width * rmhd_chunk.height;
		int* pixels = reinterpret_cast<int*>(malloc(total_bmap_pixels_size));
		for (int i = 0; i < total_bmap_pixels_size; i++)
			pixels[i] = trns_chunk.trns_val;

		uint64_t remaining_pixels = total_bmap_pixels_size;

		FILE* rfile = nullptr;

		// Open the file.
		fopen_s(&rfile, "D:/ekkes/download.bmp", "rb");
		if (rfile == nullptr)
		{
			return;
		}

		BMAP::BMPHead bmpHead;
		BMAP::BMPHeader header;

		int infohd_size = sizeof(BMAP::BMPHeader);
		int offbits = sizeof(BMAP::BMPHead) + infohd_size + 1024;
		int bitcount = 8;
		int sizeimage = total_bmap_pixels_size;
		int fsize = offbits + sizeimage;
		int xpels = 0;
		int ypels = 0;
		int clrused = 256;
		int clrimp = 256;

		header.infohd_width = rmhd_chunk.width;
		header.infohd_width = rmhd_chunk.height;
		bmpHead.filehd_size = fsize;
		bmpHead.filehd_offbits = offbits;
		header.infohd_size = infohd_size;
		header.infohd_bitcount = bitcount;
		header.infohd_sizeimage = sizeimage;
		header.infohd_xpelsm = xpels;
		header.infohd_ypelsm = ypels;
		header.infohd_clrused = clrused;
		header.infohd_clrimp = clrimp;

		fwrite(&bmpHead, sizeof(bmpHead), 1, rfile);
		fwrite(&header, sizeof(header), 1, rfile);

		//unsigned char colortable[1024];
		//// fill colortable with colors from palette
		//for (int i = 0; i < 1024; i += 4)
		//{
		//	colortable[i + 3] = 0;
		//	int color = bmpdat.get_palette()[i / 4];
		//	colortable[i + 2] = color & 0xFF;
		//	colortable[i + 1] = (color & 0xFF00) >> 8;
		//	colortable[i] = (color & 0xFF0000) >> 16;
		//}
		//fwrite(colortable, 256 * 4, 1, rfile);

		//System::Windows::Forms::PictureBox^ pictureBox;
		//pictureBox = (gcnew System::Windows::Forms::PictureBox());

		//array<unsigned char>^ ar = gcnew array<unsigned char>(size);

		//for (size_t i = 0; i < size; i++)
		//	ar[i] = bmap_chunk.data[i];

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
}