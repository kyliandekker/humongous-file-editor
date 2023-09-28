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

#include "functions/types/BMAPTab.h"
#include "functions/types/DIGITab.h"
#include "functions/types/SONGTab.h"
#include "functions/types/TALKTab.h"
#include "functions/ChunkFunctions.h"

namespace HumongousFileEditor
{
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
			has_sdat = SONGTab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0)
			has_sdat = TALKTab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::DIGI_CHUNK_ID) == 0)
			has_sdat = DIGITab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);

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

		switch (btn->resourceType)
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
					has_sdat = SONGTab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);
				else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0)
					has_sdat = TALKTab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);
				else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::DIGI_CHUNK_ID) == 0)
					has_sdat = DIGITab::GetData(fc, btn->offset, sdat_chunk, hshd_chunk);

				if (!has_sdat)
				{
					LOGF(logger::LOGSEVERITY_ERROR, "Could not process file type of file \"%s\" because it has no SDAT chunk.", fc->path.c_str());
					return;
				}

				if (SoundTab::SaveSound(hshd_chunk, sdat_chunk))
				{
					System::Windows::Forms::MessageBox::Show("Successfully exported file.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
					return;
				}
				break;
			}
			case files::ResourceType::Local_Script:
			case files::ResourceType::Global_Script:
			case files::ResourceType::Verb_Script:
			{
				break;
			}
			case files::ResourceType::RoomBackground:
			{
				img_info info;
				if (!GetRoomBackgroundData(fc, btn->offset, info))
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

					delete[] path;

					if (ofn.nFilterIndex == 1)
					{
						if (!utils::ends_with(save_path_s, ".bmp"))
							save_path_s += ".bmp";
						stbi_write_bmp(save_path_s.c_str(), static_cast<int>(info.width), static_cast<int>(info.height), static_cast<int>(info.channels), info.data.data());
					}
					else
					{
						if (!utils::ends_with(save_path_s, ".png"))
							save_path_s += ".png";
						stbi_write_png(save_path_s.c_str(), static_cast<int>(info.width), static_cast<int>(info.height), static_cast<int>(info.channels), info.data.data(), static_cast<int>(info.width * info.channels));
					}

					System::Windows::Forms::MessageBox::Show("Successfully exported file.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
					return;
				}

				break;
			}
			case files::ResourceType::RoomImage:
			{
				img_info info;
				if (btn->special)
				{
					if (!GetRoomImageLayerData(fc, btn->offset, info))
						return;
				}
				else
				{
					if (!GetRoomImageData(fc, btn->offset, info))
						return;
				}

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
						stbi_write_bmp(save_path_s.c_str(), static_cast<int>(info.width), static_cast<int>(info.height), static_cast<int>(info.channels), info.data.data());
					}
					else
					{
						if (!utils::ends_with(save_path_s, ".png"))
							save_path_s += ".png";
						stbi_write_png(save_path_s.c_str(), static_cast<int>(info.width), static_cast<int>(info.height), static_cast<int>(info.channels), info.data.data(), static_cast<int>(info.width* info.channels));
					}

					System::Windows::Forms::MessageBox::Show("Successfully exported file.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
					return;
				}

				break;
			}
			default:
			{
				break;
			}
		}
	}

	System::Void TabFunctions::ReplaceButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		HumongousButton^ btn = (HumongousButton^)sender;

		chunk_reader::FileContainer* fc = files::FILES.getFile(btn->fileType);

		if (fc == nullptr)
			return;

		bool success = false;
		switch (btn->resourceType)
		{
			case files::ResourceType::Song:
			{
				success = SONGTab::ReplaceResource(fc, btn->offset);
				break;
			}
			case files::ResourceType::Talkie:
			{
				success = TALKTab::ReplaceResource(fc, btn->offset);
				break;
			}
		}

		if (!success)
		{
			System::Windows::Forms::MessageBox::Show("Could not replace resource.", "Replacing failed", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
			return;
		}

		HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
		form->entryView->Nodes->Clear();
		form->tabControl1->Controls->Clear();

		HumongousFileEditor::chunk_reader::ResourceGatherer rg;
		rg.Read(fc);

		System::Windows::Forms::MessageBox::Show("Successfully replaced resource.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
		form->toolProgressBar->Value = 0;
	}

	void TabFunctions::AddTab(HumongousNode^ node, System::Windows::Forms::TabControl^ tabControl)
	{
		chunk_reader::FileContainer* fc = files::FILES.getFile(node->fileType);

		if (fc == nullptr)
			return;

		chunk_reader::ChunkInfo chunk = fc->GetChunkInfo(node->offset);

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

		switch (node->resourceType)
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
			case files::ResourceType::Global_Script:
			{
				GetGlobalScript(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
				break;
			}
			case files::ResourceType::Local_Script:
			{
				//GetGlobalScript(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
				break;
			}
			case files::ResourceType::Verb_Script:
			{
				//GetGlobalScript(fc, node->offset, newTab, propertyGrid, actionPanel, posX, posY);
				break;
			}
			case files::ResourceType::RoomBackground:
			{
				GetRoomBackground(fc, node->offset, newTab, propertyGrid, actionPanel, propertyPanel, posX, posY);
				break;
			}
			case files::ResourceType::RoomImage:
			{
				if (!node->special)
					GetRoomImage(fc, node->offset, newTab, propertyGrid, actionPanel, propertyPanel, posX, posY);
				else
					GetRoomImageLayer(fc, node->offset, newTab, propertyGrid, actionPanel, propertyPanel, posX, posY);
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

		HumongousButton^ replaceButton;
		replaceButton = (gcnew HumongousButton());

		replaceButton->Location = System::Drawing::Point(232, 53);
		replaceButton->Name = gcnew System::String("ReplaceSong_") + gcnew System::String(newTab->Name);
		replaceButton->Size = System::Drawing::Size(75, 23);
		replaceButton->TabIndex = 2;
		replaceButton->Text = L"Replace";
		replaceButton->offset = node->offset;
		replaceButton->fileType = fc->fileType;
		replaceButton->resourceType = node->resourceType;
		replaceButton->UseVisualStyleBackColor = true;
		replaceButton->Click += gcnew System::EventHandler(this, &TabFunctions::ReplaceButton_Click);

		replaceButton->ResumeLayout(false);
		actionPanel->Controls->Add(replaceButton);

		// Construct export button.
		HumongousButton^ exportButton;
		exportButton = (gcnew HumongousButton());

		exportButton->Location = System::Drawing::Point(232, 53);
		exportButton->Name = gcnew System::String("Export_") + gcnew System::String(newTab->Name);
		exportButton->Size = System::Drawing::Size(75, 23);
		exportButton->TabIndex = 2;
		exportButton->offset = node->offset;
		exportButton->special = node->special;
		exportButton->fileType = node->fileType;
		exportButton->resourceType = node->resourceType;
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
		if (!TALKTab::GetData(fc, offset, sdat_chunk, hshd_chunk))
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
		if (!DIGITab::GetData(fc, offset, sdat_chunk, hshd_chunk))
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
		if (!SONGTab::GetData(fc, offset, sdat_chunk, hshd_chunk))
			return;

		AddInfoRow("Sample Rate", gcnew System::String(std::to_string(hshd_chunk.sample_rate).c_str()), propertyGrid, posX, posY);
		AddInfoRow("Size (in bytes)", gcnew System::String(std::to_string(sdat_chunk.ChunkSize()).c_str()), propertyGrid, posX, posY);

		AddSoundButtons(tab, offset, fc->fileType, panel);
	}

	void TabFunctions::GetGlobalScript(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Script"), propertyGrid, posX, posY);

		chunk_reader::ChunkInfo chunkInfo = fc->GetChunkInfo(offset);
		if (utils::chunkcmp(chunkInfo.chunk_id, chunk_reader::SCRP_CHUNK_ID) == 0 ||
			utils::chunkcmp(chunkInfo.chunk_id, chunk_reader::ENCD_CHUNK_ID) == 0 ||
			utils::chunkcmp(chunkInfo.chunk_id, chunk_reader::EXCD_CHUNK_ID) == 0
			)
			AddInfoRow("Script Type", gcnew System::String("Global"), propertyGrid, posX, posY);
		else if (
			utils::chunkcmp(chunkInfo.chunk_id, chunk_reader::LSCR_CHUNK_ID) == 0 ||
			utils::chunkcmp(chunkInfo.chunk_id, chunk_reader::LSC2_CHUNK_ID) == 0
			)
			AddInfoRow("Script Type", gcnew System::String("Local"), propertyGrid, posX, posY);
		else if (
			utils::chunkcmp(chunkInfo.chunk_id, chunk_reader::VERB_CHUNK_ID) == 0
			)
			AddInfoRow("Script Type", gcnew System::String("Verb"), propertyGrid, posX, posY);

		chunk_reader::SCRP_Chunk scrp_chunk;
		memcpy(&scrp_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::SCRP_Chunk) - sizeof(scrp_chunk.data));
		scrp_chunk.data = utils::add(fc->data, offset + sizeof(chunk_reader::HumongousHeader));

		std::string t;
		for (size_t i = 0; i < scrp_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader); i++)
			t += scrp_chunk.data[i];
		AddInfoRow("Script", gcnew System::String(t.c_str()), propertyGrid, posX, posY);
	}

	bool TabFunctions::GetRoomBackgroundData(chunk_reader::FileContainer*& fc, size_t offset, img_info& info)
	{
		std::vector<chunk_reader::ChunkInfo> children = fc->GetChildren(offset);
		if (children.size() == 0)
			return false;

		size_t bsmap_offset = -1;
		for (size_t i = 0; i < children.size(); i++)
			if (utils::chunkcmp(children[i].chunk_id, chunk_reader::BMAP_CHUNK_ID) == 0 || utils::chunkcmp(children[i].chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
				bsmap_offset = children[i].offset;

		if (bsmap_offset < 0)
			return false;

		chunk_reader::BMAP_Chunk bmap_chunk;
		size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
		memcpy(&bmap_chunk, utils::add(fc->data, bsmap_offset), header_size);
		bmap_chunk.data = utils::add(fc->data, bsmap_offset + header_size);
		size_t bmap_size = bmap_chunk.ChunkSize() - header_size;

		size_t rmim_offset = fc->GetParent(offset).offset;
		chunk_reader::RMIM_Chunk rmim_chunk;
		memcpy(&rmim_chunk, utils::add(fc->data, rmim_offset), sizeof(chunk_reader::RMIM_Chunk));

		std::vector<chunk_reader::ChunkInfo> rmda_children = fc->GetChildren(fc->GetParent(rmim_offset).offset);
		size_t apal_offset = -1;
		size_t rmhd_offset = -1;
		for (size_t i = 0; i < rmda_children.size(); i++)
		{
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::APAL_CHUNK_ID) == 0)
				apal_offset = rmda_children[i].offset;
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::RMHD_CHUNK_ID) == 0)
				rmhd_offset = rmda_children[i].offset;
		}

		if (rmhd_offset < 0)
			return false;

		chunk_reader::RMHD_Chunk rmhd_chunk;
		memcpy(&rmhd_chunk, utils::add(fc->data, rmhd_offset), sizeof(chunk_reader::RMHD_Chunk));

		if (apal_offset < 0)
			return false;

		chunk_reader::APAL_Chunk apal_chunk;
		header_size = sizeof(chunk_reader::APAL_Chunk);
		memcpy(&apal_chunk, utils::add(fc->data, apal_offset), header_size);
		size_t apal_size = apal_chunk.ChunkSize() - header_size;

		return BMAPTab::GetDataBMAP(fc, bmap_chunk, apal_chunk, bmap_chunk.data[0], rmhd_chunk.width, rmhd_chunk.height, info);
	}

	void TabFunctions::GetRoomBackground(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY)
	{
		img_info info;
		if (!GetRoomBackgroundData(fc, offset, info))
			return;

		size_t rmim_offset = fc->GetParent(offset).offset;
		std::vector<chunk_reader::ChunkInfo> rmda_children = fc->GetChildren(fc->GetParent(rmim_offset).offset);
		size_t apal_offset = -1;
		size_t rmhd_offset = -1;
		for (size_t i = 0; i < rmda_children.size(); i++)
		{
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::APAL_CHUNK_ID) == 0)
				apal_offset = rmda_children[i].offset;
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::RMHD_CHUNK_ID) == 0)
				rmhd_offset = rmda_children[i].offset;
		}

		if (rmhd_offset < 0)
			return;

		chunk_reader::RMHD_Chunk rmhd_chunk;
		memcpy(&rmhd_chunk, utils::add(fc->data, rmhd_offset), sizeof(chunk_reader::RMHD_Chunk));

		AddInfoRow("Type", gcnew System::String("Room Image"), propertyGrid, posX, posY);

		AddInfoRow("Width", gcnew System::String(std::to_string(rmhd_chunk.width).c_str()), propertyGrid, posX, posY);
		AddInfoRow("Height", gcnew System::String(std::to_string(rmhd_chunk.height).c_str()), propertyGrid, posX, posY);

		propertyGrid->Dock = System::Windows::Forms::DockStyle::Top;
		propertyGrid->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

		System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(static_cast<int>(rmhd_chunk.width), static_cast<int>(info.height));

		int cur = 0;
		for (size_t i = 0; i < info.size; i += info.channels, cur++)
		{
			int y = cur / static_cast<int>(rmhd_chunk.width);
			int x = cur % static_cast<int>(rmhd_chunk.width);
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
	}

	bool TabFunctions::GetRoomImageData(chunk_reader::FileContainer*& fc, size_t offset, img_info& info)
	{
		std::vector<chunk_reader::ChunkInfo> children = fc->GetChildren(offset);
		if (children.size() == 0)
			return false;

		size_t obim_offset = fc->GetParent(offset).offset;
		chunk_reader::OBIM_Chunk obim_chunk;
		memcpy(&obim_chunk, utils::add(fc->data, obim_offset), sizeof(chunk_reader::IMHD_Chunk));

		size_t bsmap_offset = -1;
		for (size_t i = 0; i < children.size(); i++)
			if (utils::chunkcmp(children[i].chunk_id, chunk_reader::BMAP_CHUNK_ID) == 0 || utils::chunkcmp(children[i].chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
				bsmap_offset = children[i].offset;

		if (bsmap_offset < 0)
			return false;

		std::vector<chunk_reader::ChunkInfo> obim_children = fc->GetChildren(obim_offset);
		size_t imhd_offset = -1;
		for (size_t i = 0; i < obim_children.size(); i++)
			if (utils::chunkcmp(obim_children[i].chunk_id, chunk_reader::IMHD_CHUNK_ID) == 0)
				imhd_offset = obim_children[i].offset;

		if (imhd_offset < 0)
			return false;

		chunk_reader::IMHD_Chunk imhd_chunk;
		memcpy(&imhd_chunk, utils::add(fc->data, imhd_offset), sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data));
		imhd_chunk.data = utils::add(fc->data, imhd_offset + (sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data)));

		std::vector<chunk_reader::ChunkInfo> rmda_children = fc->GetChildren(fc->GetParent(obim_offset).offset);
		size_t apal_offset = -1;
		for (size_t i = 0; i < rmda_children.size(); i++)
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::APAL_CHUNK_ID) == 0)
				apal_offset = rmda_children[i].offset;

		if (apal_offset < 0)
			return false;

		chunk_reader::APAL_Chunk apal_chunk;
		size_t header_size = sizeof(chunk_reader::APAL_Chunk);
		memcpy(&apal_chunk, utils::add(fc->data, apal_offset), header_size);
		size_t apal_size = apal_chunk.ChunkSize() - header_size;

		chunk_reader::ChunkInfo bsmap = fc->GetChunkInfo(bsmap_offset);

		info.x = imhd_chunk.x;
		info.y = imhd_chunk.y;

		if (utils::chunkcmp(bsmap.chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
		{
			chunk_reader::SMAP_Chunk smap_chunk;
			size_t header_size = sizeof(chunk_reader::SMAP_Chunk) - sizeof(smap_chunk.data); // Pointer in the SMAP class is size 8 and needs to be deducted.
			memcpy(&smap_chunk, utils::add(fc->data, bsmap_offset), header_size);
			smap_chunk.data = utils::add(fc->data, bsmap_offset + header_size);

			return BMAPTab::GetDataSMAP(fc, obim_chunk, imhd_chunk.width, imhd_chunk.height, smap_chunk, apal_chunk, info);
		}
		else
		{
			chunk_reader::BMAP_Chunk bmap_chunk;
			size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
			memcpy(&bmap_chunk, utils::add(fc->data, bsmap_offset), header_size);
			bmap_chunk.data = utils::add(fc->data, bsmap_offset + header_size);
			size_t bmap_size = bmap_chunk.ChunkSize() - header_size;

			return BMAPTab::GetDataBMAP(fc, bmap_chunk, apal_chunk, bmap_chunk.fill_color, imhd_chunk.width, imhd_chunk.height, info);
		}

		return false;
	}

	void TabFunctions::GetRoomImage(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY)
	{
		img_info info;
		if (!GetRoomImageData(fc, offset, info))
			return;

		AddInfoRow("Type", gcnew System::String("Room Image"), propertyGrid, posX, posY);

		AddInfoRow("Width", gcnew System::String(std::to_string(info.width).c_str()), propertyGrid, posX, posY);
		AddInfoRow("Height", gcnew System::String(std::to_string(info.height).c_str()), propertyGrid, posX, posY);
		AddInfoRow("x", gcnew System::String(std::to_string(info.x).c_str()), propertyGrid, posX, posY);
		AddInfoRow("y", gcnew System::String(std::to_string(info.y).c_str()), propertyGrid, posX, posY);

		propertyGrid->Dock = System::Windows::Forms::DockStyle::Top;
		propertyGrid->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

		System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(static_cast<int>(info.width), static_cast<int>(info.height));

		int cur = 0;
		for (size_t i = 0; i < info.size; i += info.channels, cur++)
		{
			int y = cur / static_cast<int>(info.width);
			int x = cur % static_cast<int>(info.width);
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
	}

	bool TabFunctions::GetRoomImageLayerData(chunk_reader::FileContainer*& fc, size_t offset, img_info& info)
	{
		img_info image_info;
		if (!GetRoomImageData(fc, offset, image_info))
			return false;

		size_t lflf_offset = fc->GetParent(fc->GetParent(fc->GetParent(offset).offset).offset).offset;
		std::vector<chunk_reader::ChunkInfo> lflf_children = fc->GetChildren(lflf_offset);
		size_t im00_offset = -1;
		for (size_t i = 0; i < lflf_children.size(); i++)
			if (utils::chunkcmp(lflf_children[i].chunk_id, chunk_reader::IM00_CHUNK_ID) == 0)
				im00_offset = lflf_children[i].offset;

		img_info background_info;
		if (!GetRoomBackgroundData(fc, im00_offset, background_info))
			return false;

		size_t obim_offset = fc->GetParent(offset).offset;
		std::vector<chunk_reader::ChunkInfo> obim_children = fc->GetChildren(obim_offset);
		size_t imhd_offset = -1;
		for (size_t i = 0; i < obim_children.size(); i++)
			if (utils::chunkcmp(obim_children[i].chunk_id, chunk_reader::IMHD_CHUNK_ID) == 0)
				imhd_offset = obim_children[i].offset;

		if (imhd_offset < 0)
			return false;

		chunk_reader::IMHD_Chunk imhd_chunk;
		memcpy(&imhd_chunk, utils::add(fc->data, imhd_offset), sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data));
		imhd_chunk.data = utils::add(fc->data, imhd_offset + (sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data)));

		int cur = 0;
		for (size_t i = 0; i < background_info.size; i += background_info.channels, cur++)
		{
			int y = cur / static_cast<int>(background_info.width);
			int x = cur % static_cast<int>(background_info.width);

			if (x >= imhd_chunk.x && x < imhd_chunk.x + imhd_chunk.width &&
				y >= imhd_chunk.y && y < imhd_chunk.y + imhd_chunk.height)
			{
				int relativeX = x - imhd_chunk.x;
				int relativeY = y - imhd_chunk.y;

				relativeY *= imhd_chunk.width;

				int index = relativeX + relativeY;

				index *= 4;

				if (image_info.data[index + 3] != 0)
				{
					background_info.data[i] = image_info.data[index];
					background_info.data[i + 1] = image_info.data[index + 1];
					background_info.data[i + 2] = image_info.data[index + 2];
					background_info.data[i + 3] = image_info.data[index + 3];
				}
			}
			else
			{
				background_info.data[i + 3] = 55;
			}
		}

		background_info.x = image_info.x;
		background_info.y = image_info.y;
		info = background_info;

		return true;
	}

	void TabFunctions::GetRoomImageLayer(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY)
	{
		img_info info;
		if (!GetRoomImageLayerData(fc, offset, info))
			return;

		AddInfoRow("Type", gcnew System::String("Room Image"), propertyGrid, posX, posY);

		AddInfoRow("Background Width", gcnew System::String(std::to_string(info.width).c_str()), propertyGrid, posX, posY);
		AddInfoRow("Background Height", gcnew System::String(std::to_string(info.height).c_str()), propertyGrid, posX, posY);
		AddInfoRow("x", gcnew System::String(std::to_string(info.x).c_str()), propertyGrid, posX, posY);
		AddInfoRow("y", gcnew System::String(std::to_string(info.y).c_str()), propertyGrid, posX, posY);

		propertyGrid->Dock = System::Windows::Forms::DockStyle::Top;
		propertyGrid->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

		System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(static_cast<int>(info.width), static_cast<int>(info.height));

		int cur = 0;
		for (size_t i = 0; i < info.size; i += info.channels, cur++)
		{
			int y = cur / static_cast<int>(info.width);
			int x = cur % static_cast<int>(info.width);
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
	}

	void TabFunctions::GetRNAM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY)
	{
		AddInfoRow("Type", gcnew System::String("Rooms"), propertyGrid, posX, posY);

		chunk_reader::RNAM_Chunk* rnam_chunk = reinterpret_cast<chunk_reader::RNAM_Chunk*>(utils::add(fc->data, offset));
		size_t rnam_end = offset + rnam_chunk->ChunkSize();
		size_t pos = offset + sizeof(chunk_reader::HumongousHeader) + sizeof(uint16_t);
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
}