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
#include "HumongousEditorForm.h"

#include "functions/types/BMAPTab.h"
#include "functions/types/DIGITab.h"
#include "functions/types/SONGTab.h"
#include "functions/types/TALKTab.h"
#include "functions/types/SCRPTab.h"
#include "functions/ChunkFunctions.h"
#include "cmd/talk_string.h"

namespace HumongousFileEditor
{
	// Adds a row of info to the info tab.
	int AddInfoRow(array<System::String^>^ test, System::Windows::Forms::DataGridView^ propertyGrid)
	{
		int i = propertyGrid->Rows->Add(test);
		for (size_t j = 0; j < test->Length - propertyGrid->Columns->Count; j++)
		{
			int index = propertyGrid->Columns->Add(gcnew System::String(std::to_string(j).c_str()), gcnew System::String(""));
			propertyGrid->Columns[index]->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
		}
		for (size_t j = 0; j < test->Length; j++)
			propertyGrid->Rows[i]->Cells[j]->ReadOnly = true;
		return i;
	}

	// Callback for the play button.
	System::Void TabFunctions::PlayButton_Click(System::Object^ sender, System::EventArgs^)
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
	System::Void TabFunctions::StopButton_Click(System::Object^, System::EventArgs^)
	{
		audioSystem.Stop();
	}

	// Callback for the export button.
	System::Void TabFunctions::NextButton_Click(System::Object^ sender, System::EventArgs^)
	{
		HumongousButton^ btn = (HumongousButton^)sender;

		HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];

		HumongousNode^ node = (HumongousNode^) btn->node->NextNode;

		if (!form->tabControl1->Controls->ContainsKey(node->Name))
			form->AddTab(node);
		form->tabControl1->SelectedIndex = form->tabControl1->Controls->IndexOfKey(node->Name);
	}

	System::Void TabFunctions::ExportButton_Click(System::Object^ sender, System::EventArgs^)
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

				std::string path;
				int choice = 1;
				if (HumongousFileEditor::abstractions::SaveWFile(path, &choice, L"\
					BMP file (*.bmp)\
					\0*.BMP;*.bmp\0\
					PNG file (*.png)\
					\0*.PNG;*.png\0"))
				{
					if (choice == 1)
					{
						if (!utils::ends_with(path, ".bmp"))
							path += ".bmp";
						stbi_write_bmp(path.c_str(), static_cast<int>(info.width), static_cast<int>(info.height), static_cast<int>(info.channels), info.data.data);
					}
					else
					{
						if (!utils::ends_with(path, ".png"))
							path += ".png";
						stbi_write_png(path.c_str(), static_cast<int>(info.width), static_cast<int>(info.height), static_cast<int>(info.channels), info.data.data, static_cast<int>(info.width * info.channels));
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


				std::string path;
				int choice = 1;
				if (HumongousFileEditor::abstractions::SaveWFile(path, &choice, L"\
					BMP file (*.bmp)\
					\0*.BMP;*.bmp\0\
					PNG file (*.png)\
					\0*.PNG;*.png\0"))
				{
					if (choice == 1)
					{
						if (!utils::ends_with(path, ".bmp"))
							path += ".bmp";
						stbi_write_bmp(path.c_str(), static_cast<int>(info.width), static_cast<int>(info.height), static_cast<int>(info.channels), info.data.data);
					}
					else
					{
						if (!utils::ends_with(path, ".png"))
							path += ".png";
						stbi_write_png(path.c_str(), static_cast<int>(info.width), static_cast<int>(info.height), static_cast<int>(info.channels), info.data.data, static_cast<int>(info.width * info.channels));
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

	System::Void TabFunctions::ReplaceButton_Click(System::Object^ sender, System::EventArgs^)
	{
		HumongousButton^ btn = (HumongousButton^)sender;

		chunk_reader::FileContainer* fc = files::FILES.getFile(btn->fileType);

		if (fc == nullptr)
			return;

		audioSystem.Stop();

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
			case files::ResourceType::SFX:
			{
				System::Windows::Forms::MessageBox::Show("WARNING: Replacing sound effects does not fully work yet. It will only work in the program itself, but will cause the game to crash.", "WARNING", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Warning);
				success = DIGITab::ReplaceResource(fc, btn->offset);
				break;
			}
			case files::ResourceType::RoomImage:
			case files::ResourceType::RoomBackground:
			{
				System::Windows::Forms::MessageBox::Show("Currently too lazy to do this.", "Nope", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Warning);
				return;
			}
		}

		if (!success)
		{
			System::Windows::Forms::MessageBox::Show("Could not replace resource.", "Replacing failed", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
			return;
		}

		HumongousFileEditor::chunk_reader::ResourceGatherer rg;
		rg.Read(fc);

		System::Windows::Forms::MessageBox::Show("Successfully replaced resource.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
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
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Pos", 
			gcnew System::String(std::to_string(node->offset).c_str())
		}, propertyGrid);

		switch (node->resourceType)
		{
			case files::ResourceType::Song:
			{
				GetSong(fc, node->offset, newTab, propertyGrid, actionPanel);
				break;
			}
			case files::ResourceType::Talkie:
			{
				GetTalk(fc, node->offset, newTab, propertyGrid, actionPanel);
				break;
			}
			case files::ResourceType::SFX:
			{
				GetDigi(fc, node->offset, newTab, propertyGrid, actionPanel);
				break;
			}
			case files::ResourceType::Global_Script:
			{
				GetGlobalScript(fc, node->offset, newTab, propertyGrid, actionPanel);
				break;
			}
			case files::ResourceType::Local_Script:
			{
				GetLocalScript(fc, node->offset, newTab, propertyGrid, actionPanel);
				break;
			}
			case files::ResourceType::Verb_Script:
			{
				//GetGlobalScript(fc, node->offset, newTab, propertyGrid, actionPanel);
				break;
			}
			case files::ResourceType::RoomBackground:
			{
				if (!node->special)
					GetRoomBackground(fc, node->offset, newTab, propertyGrid, actionPanel, propertyPanel);
				else
					GetRoomPalette(fc, node->offset, newTab, propertyGrid, actionPanel, propertyPanel);
				break;
			}
			case files::ResourceType::RoomImage:
			{
				if (!node->special)
					GetRoomImage(fc, node->offset, newTab, propertyGrid, actionPanel, propertyPanel);
				else
					GetRoomImageLayer(fc, node->offset, newTab, propertyGrid, actionPanel, propertyPanel);
				break;
			}
			case files::ResourceType::Room:
			{
				GetRNAM(fc, node->offset, newTab, propertyGrid, actionPanel);
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

		// Construct export button.
		HumongousButton^ nextButton;
		nextButton = (gcnew HumongousButton());

		nextButton->Location = System::Drawing::Point(232, 53);
		nextButton->Name = gcnew System::String("Next_") + gcnew System::String(newTab->Name);
		nextButton->Size = System::Drawing::Size(75, 23);
		nextButton->TabIndex = 2;
		nextButton->offset = node->offset;
		nextButton->node = node;
		nextButton->special = node->special;
		nextButton->fileType = node->fileType;
		nextButton->resourceType = node->resourceType;
		nextButton->Text = L"Next";
		nextButton->UseVisualStyleBackColor = true;
		nextButton->Click += gcnew System::EventHandler(this, &TabFunctions::NextButton_Click);

		nextButton->ResumeLayout(false);
		actionPanel->Controls->Add(nextButton);

		newTab->ResumeLayout(false);

		newTab->Controls->Add(actionPanel);
		tabControl->Controls->Add(newTab);
	}

	System::Void TabFunctions::DoubleClick(System::Object^, System::Windows::Forms::MouseEventArgs^)
	{
		return System::Void();
	}

	void TabFunctions::GetTalk(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel)
	{
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Type", 
			gcnew System::String("Talk")
		}, propertyGrid);

		chunk_reader::SDAT_Chunk sdat_chunk;
		chunk_reader::HSHD_Chunk hshd_chunk;
		if (!TALKTab::GetData(fc, offset, sdat_chunk, hshd_chunk))
			return;

		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Sample Rate", 
			gcnew System::String(std::to_string(hshd_chunk.sample_rate).c_str())
		}, propertyGrid);
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Size (in bytes)", 
			gcnew System::String(std::to_string(sdat_chunk.ChunkSize()).c_str())
		}, propertyGrid);

		AddSoundButtons(tab, offset, fc->fileType, panel);
	}

	void TabFunctions::GetSong(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel)
	{
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Type",
				gcnew System::String("Song")
		}, propertyGrid);

		chunk_reader::SDAT_Chunk sdat_chunk;
		chunk_reader::HSHD_Chunk hshd_chunk;
		if (!SONGTab::GetData(fc, offset, sdat_chunk, hshd_chunk))
			return;

		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Sample Rate",
		}, propertyGrid);
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Size (in bytes)",
				gcnew System::String(std::to_string(sdat_chunk.ChunkSize()).c_str())
		}, propertyGrid);

		AddSoundButtons(tab, offset, fc->fileType, panel);
	}

	void TabFunctions::GetDigi(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel)
	{
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Type", 
			gcnew System::String("Sfx")
		}, propertyGrid);

		chunk_reader::SDAT_Chunk sdat_chunk;
		chunk_reader::HSHD_Chunk hshd_chunk;
		if (!DIGITab::GetData(fc, offset, sdat_chunk, hshd_chunk))
			return;

		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Sample Rate", 
		}, propertyGrid);
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Size (in bytes)",
			gcnew System::String(std::to_string(sdat_chunk.ChunkSize()).c_str())
		}, propertyGrid);

		AddSoundButtons(tab, offset, fc->fileType, panel);
	}

	void TabFunctions::GetGlobalScript(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^)
	{
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Type", 
			gcnew System::String("Script")
		}, propertyGrid);
		
		std::vector<talk_instruction> instructions;
		if (!SCRPTab::GetData(fc, offset, instructions))
			return;

		int amount = 0;
		for (size_t i = 0; i < instructions.size(); i++)
		{
			talk_instruction& instruction = instructions[i];
			if (instruction.args.ArgSize() > amount)
			{
				int d = instruction.args.ArgSize();
				amount = d;
			}
		}

		for (size_t i = 0; i < instructions.size(); i++)
		{
			talk_instruction& instruction = instructions[i];

			array<System::String^>^ arr = gcnew array<System::String^>(amount + 1);
			for (size_t j = 0; j < arr->Length; j++)
				arr[j] = gcnew System::String("");
			
			arr[0] = gcnew System::String(std::string(std::to_string(i) + "_" + instructions[i].name).c_str());
			for (size_t j = 0; j < instruction.args.args.size(); j++)
				arr[j + 1] = gcnew System::String(instruction.args[j].str.c_str());

			AddInfoRow(arr, propertyGrid);
		}
	}

	void TabFunctions::GetLocalScript(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^)
	{
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Type", 
			gcnew System::String("Script")
		}, propertyGrid);

		std::vector<talk_instruction> instructions;
		if (!SCRPTab::GetData(fc, offset, instructions))
			return;

		for (size_t i = 0; i < instructions.size(); i++)
		{
			talk_instruction& instruction = instructions[i];
			AddInfoRow(gcnew array<System::String^>(2)
			{
				gcnew System::String(std::string(std::to_string(i) + "_" + instructions[i].name).c_str()), 
				gcnew System::String("")
			}, propertyGrid);
		}
	}

	bool TabFunctions::GetRoomBackgroundData(chunk_reader::FileContainer*& fc, size_t offset, img_info& info)
	{
		std::vector<chunk_reader::ChunkInfo> children = fc->GetChildren(offset);
		if (children.size() == 0)
			return false;

		int32_t bsmap_offset = -1;
		for (size_t i = 0; i < children.size(); i++)
			if (utils::chunkcmp(children[i].chunk_id, chunk_reader::BMAP_CHUNK_ID) == 0 || utils::chunkcmp(children[i].chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
				bsmap_offset = static_cast<int32_t>(children[i].offset);

		if (bsmap_offset < 0)
			return false;

		chunk_reader::BMAP_Chunk bmap_chunk;
		size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
		memcpy(&bmap_chunk, utils::add(fc->data, bsmap_offset), header_size);
		bmap_chunk.data = utils::add(fc->data, bsmap_offset + header_size);

		size_t rmim_offset = fc->GetParent(offset).offset;
		chunk_reader::RMIM_Chunk rmim_chunk;
		memcpy(&rmim_chunk, utils::add(fc->data, rmim_offset), sizeof(chunk_reader::RMIM_Chunk));

		std::vector<chunk_reader::ChunkInfo> rmda_children = fc->GetChildren(fc->GetParent(rmim_offset).offset);
		int32_t apal_offset = -1;
		int32_t rmhd_offset = -1;
		for (size_t i = 0; i < rmda_children.size(); i++)
		{
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::APAL_CHUNK_ID) == 0)
				apal_offset = static_cast<int32_t>(rmda_children[i].offset);
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::RMHD_CHUNK_ID) == 0)
				rmhd_offset = static_cast<int32_t>(rmda_children[i].offset);
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

		return BMAPTab::GetDataBMAP(fc, bmap_chunk, apal_chunk, bmap_chunk.data[0], rmhd_chunk.width, rmhd_chunk.height, info);
	}

	void TabFunctions::GetRoomBackground(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel)
	{
		img_info info;
		if (!GetRoomBackgroundData(fc, offset, info))
			return;

		size_t rmim_offset = fc->GetParent(offset).offset;
		std::vector<chunk_reader::ChunkInfo> rmda_children = fc->GetChildren(fc->GetParent(rmim_offset).offset);
		int32_t apal_offset = -1;
		int32_t rmhd_offset = -1;
		for (size_t i = 0; i < rmda_children.size(); i++)
		{
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::APAL_CHUNK_ID) == 0)
				apal_offset = static_cast<int32_t>(rmda_children[i].offset);
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::RMHD_CHUNK_ID) == 0)
				rmhd_offset = static_cast<int32_t>(rmda_children[i].offset);
		}

		if (rmhd_offset < 0)
			return;

		chunk_reader::RMHD_Chunk rmhd_chunk;
		memcpy(&rmhd_chunk, utils::add(fc->data, rmhd_offset), sizeof(chunk_reader::RMHD_Chunk));

		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Type", 
			gcnew System::String("Room Image")
		}, propertyGrid);

		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Width", 
			gcnew System::String(std::to_string(rmhd_chunk.width).c_str())
		}, propertyGrid);
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Height", 
			gcnew System::String(std::to_string(rmhd_chunk.height).c_str())
		}, propertyGrid);

		propertyGrid->Dock = System::Windows::Forms::DockStyle::Top;
		propertyGrid->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

		System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(static_cast<int>(rmhd_chunk.width), static_cast<int>(info.height));

		int cur = 0;
		for (size_t i = 0; i < info.size; i += info.channels, cur++)
		{
			int y = cur / static_cast<int>(rmhd_chunk.width);
			int x = cur % static_cast<int>(rmhd_chunk.width);
			if (info.channels < 4)
				bmp->SetPixel(x, y, System::Drawing::Color::FromArgb(255, info.data.data[i], info.data.data[i + 1], info.data.data[i + 2]));
			else
				bmp->SetPixel(x, y, System::Drawing::Color::FromArgb(info.data.data[i + 3], info.data.data[i], info.data.data[i + 1], info.data.data[i + 2]));
		}

		System::Windows::Forms::PictureBox^ pictureBox;
		pictureBox = (gcnew System::Windows::Forms::PictureBox());
		pictureBox->Dock = System::Windows::Forms::DockStyle::Top;
		pictureBox->Location = System::Drawing::Point(0, propertyGrid->Height);
		pictureBox->Name = L"Action Panel";
		pictureBox->Image = bmp;
		pictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
		pictureBox->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

		propertyPanel->Controls->Add(pictureBox);
	}

	void TabFunctions::GetRoomPalette(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel)
	{
		std::vector<chunk_reader::ChunkInfo> children = fc->GetChildren(offset);
		if (children.size() == 0)
			return;

		size_t rmim_offset = fc->GetParent(offset).offset;
		chunk_reader::RMIM_Chunk rmim_chunk;
		memcpy(&rmim_chunk, utils::add(fc->data, rmim_offset), sizeof(chunk_reader::RMIM_Chunk));

		std::vector<chunk_reader::ChunkInfo> rmda_children = fc->GetChildren(fc->GetParent(rmim_offset).offset);
		size_t apal_offset = -1;
		for (size_t i = 0; i < rmda_children.size(); i++)
		{
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::APAL_CHUNK_ID) == 0)
				apal_offset = rmda_children[i].offset;
		}

		if (apal_offset < 0)
			return;

		chunk_reader::APAL_Chunk apal_chunk;
		size_t header_size = sizeof(chunk_reader::APAL_Chunk);
		memcpy(&apal_chunk, utils::add(fc->data, apal_offset), header_size);
		size_t apal_size = apal_chunk.ChunkSize() - header_size;

		size_t width = 1024, height = 1024;

		size_t width_per_cell = width / 16, height_per_cell = height / 16;

		System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(static_cast<int>(width), static_cast<int>(height));

		for (size_t color = 0; color < 256; color++)
		{
			for (size_t y = 0; y < height_per_cell; y++)
			{
				for (size_t x = 0; x < width_per_cell; x++)
				{
					size_t abs_x = x + (color % 16 * width_per_cell);
					size_t abs_y = y + (color / 16 * height_per_cell);

					size_t color_index = color * 3;
					bmp->SetPixel(abs_x, abs_y, System::Drawing::Color::FromArgb(255, apal_chunk.data[color_index], apal_chunk.data[color_index + 1], apal_chunk.data[color_index + 2]));
				}
			}
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

		int32_t bsmap_offset = -1;
		for (size_t i = 0; i < children.size(); i++)
			if (utils::chunkcmp(children[i].chunk_id, chunk_reader::BMAP_CHUNK_ID) == 0 || utils::chunkcmp(children[i].chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
				bsmap_offset = static_cast<int32_t>(children[i].offset);

		if (bsmap_offset < 0)
			return false;

		std::vector<chunk_reader::ChunkInfo> obim_children = fc->GetChildren(obim_offset);
		int32_t imhd_offset = -1;
		for (size_t i = 0; i < obim_children.size(); i++)
			if (utils::chunkcmp(obim_children[i].chunk_id, chunk_reader::IMHD_CHUNK_ID) == 0)
				imhd_offset = static_cast<int32_t>(obim_children[i].offset);

		if (imhd_offset < 0)
			return false;

		chunk_reader::IMHD_Chunk imhd_chunk;
		memcpy(&imhd_chunk, utils::add(fc->data, imhd_offset), sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data));
		imhd_chunk.data = utils::add(fc->data, imhd_offset + (sizeof(chunk_reader::IMHD_Chunk) - sizeof(imhd_chunk.data)));

		std::vector<chunk_reader::ChunkInfo> rmda_children = fc->GetChildren(fc->GetParent(obim_offset).offset);
		int32_t apal_offset = -1;
		for (size_t i = 0; i < rmda_children.size(); i++)
			if (utils::chunkcmp(rmda_children[i].chunk_id, chunk_reader::APAL_CHUNK_ID) == 0)
				apal_offset = static_cast<int32_t>(rmda_children[i].offset);

		if (apal_offset < 0)
			return false;

		chunk_reader::APAL_Chunk apal_chunk;
		size_t header_size = sizeof(chunk_reader::APAL_Chunk);
		memcpy(&apal_chunk, utils::add(fc->data, apal_offset), header_size);

		chunk_reader::ChunkInfo bsmap = fc->GetChunkInfo(bsmap_offset);

		info.x = imhd_chunk.x;
		info.y = imhd_chunk.y;

		if (utils::chunkcmp(bsmap.chunk_id, chunk_reader::SMAP_CHUNK_ID) == 0)
		{
			chunk_reader::SMAP_Chunk smap_chunk;
			size_t smap_header_size = sizeof(chunk_reader::SMAP_Chunk) - sizeof(smap_chunk.data); // Pointer in the SMAP class is size 8 and needs to be deducted.
			memcpy(&smap_chunk, utils::add(fc->data, bsmap_offset), smap_header_size);
			smap_chunk.data = utils::add(fc->data, bsmap_offset + smap_header_size);

			return BMAPTab::GetDataSMAP(fc, imhd_chunk.width, imhd_chunk.height, smap_chunk, apal_chunk, info);
		}
		else
		{
			chunk_reader::BMAP_Chunk bmap_chunk;
			size_t bmap_header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
			memcpy(&bmap_chunk, utils::add(fc->data, bsmap_offset), bmap_header_size);
			bmap_chunk.data = utils::add(fc->data, bsmap_offset + bmap_header_size);

			return BMAPTab::GetDataBMAP(fc, bmap_chunk, apal_chunk, bmap_chunk.fill_color, imhd_chunk.width, imhd_chunk.height, info);
		}

		return false;
	}

	void TabFunctions::GetRoomImage(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel)
	{
		img_info info;
		if (!GetRoomImageData(fc, offset, info))
			return;

		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Type", 
			gcnew System::String("Room Image")
		}, propertyGrid);

		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Width", 
			gcnew System::String(std::to_string(info.width).c_str())
		}, propertyGrid);
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Height", 
			gcnew System::String(std::to_string(info.height).c_str())
		}, propertyGrid);
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"x", 
			gcnew System::String(std::to_string(info.x).c_str())
		}, propertyGrid);
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"y", 
			gcnew System::String(std::to_string(info.y).c_str())
		}, propertyGrid);

		propertyGrid->Dock = System::Windows::Forms::DockStyle::Top;
		propertyGrid->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

		System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(static_cast<int>(info.width), static_cast<int>(info.height));

		int cur = 0;
		for (size_t i = 0; i < info.size; i += info.channels, cur++)
		{
			int y = cur / static_cast<int>(info.width);
			int x = cur % static_cast<int>(info.width);
			bmp->SetPixel(x, y, System::Drawing::Color::FromArgb(info.data.data[i + 3], info.data.data[i], info.data.data[i + 1], info.data.data[i + 2]));
		}

		System::Windows::Forms::PictureBox^ pictureBox;
		pictureBox = (gcnew System::Windows::Forms::PictureBox());
		pictureBox->Dock = System::Windows::Forms::DockStyle::Top;
		pictureBox->Location = System::Drawing::Point(0, propertyGrid->Height);
		pictureBox->Name = L"Action Panel";
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
		int32_t im00_offset = -1;
		for (size_t i = 0; i < lflf_children.size(); i++)
			if (utils::chunkcmp(lflf_children[i].chunk_id, chunk_reader::IM00_CHUNK_ID) == 0)
				im00_offset = static_cast<int32_t>(lflf_children[i].offset);

		img_info background_info;
		if (!GetRoomBackgroundData(fc, im00_offset, background_info))
			return false;

		size_t obim_offset = fc->GetParent(offset).offset;
		std::vector<chunk_reader::ChunkInfo> obim_children = fc->GetChildren(obim_offset);
		int32_t imhd_offset = -1;
		for (size_t i = 0; i < obim_children.size(); i++)
			if (utils::chunkcmp(obim_children[i].chunk_id, chunk_reader::IMHD_CHUNK_ID) == 0)
				imhd_offset = static_cast<int32_t>(obim_children[i].offset);

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

				if (image_info.data.data[index + 3] != 0)
				{
					background_info.data.data[i] = image_info.data.data[index];
					background_info.data.data[i + 1] = image_info.data.data[index + 1];
					background_info.data.data[i + 2] = image_info.data.data[index + 2];
					background_info.data.data[i + 3] = image_info.data.data[index + 3];
				}
			}
			else
			{
				background_info.data.data[i + 3] = 55;
			}
		}

		background_info.x = image_info.x;
		background_info.y = image_info.y;
		info = background_info;

		return true;
	}

	void TabFunctions::GetRoomImageLayer(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^, System::Windows::Forms::Panel^ propertyPanel)
	{
		img_info info;
		if (!GetRoomImageLayerData(fc, offset, info))
			return;

		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Type",
		}, propertyGrid);

		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Background Width",
			gcnew System::String(std::to_string(info.width).c_str())
		}, propertyGrid);
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Background Height", 
			gcnew System::String(std::to_string(info.height).c_str())
		}, propertyGrid);
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"x", 
			gcnew System::String(std::to_string(info.x).c_str())
		}, propertyGrid);
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"y", 
			gcnew System::String(std::to_string(info.y).c_str())
		}, propertyGrid);

		System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(static_cast<int>(info.width), static_cast<int>(info.height));

		int cur = 0;
		for (size_t i = 0; i < info.size; i += info.channels, cur++)
		{
			int y = cur / static_cast<int>(info.width);
			int x = cur % static_cast<int>(info.width);
			bmp->SetPixel(x, y, System::Drawing::Color::FromArgb(info.data.data[i + 3], info.data.data[i], info.data.data[i + 1], info.data.data[i + 2]));
		}

		System::Windows::Forms::PictureBox^ pictureBox;
		pictureBox = (gcnew System::Windows::Forms::PictureBox());
		pictureBox->Dock = System::Windows::Forms::DockStyle::Top;
		pictureBox->Location = System::Drawing::Point(0, propertyGrid->Height);
		pictureBox->Name = L"Action Panel";
		pictureBox->Image = bmp;
		pictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
		pictureBox->Size = System::Drawing::Size(propertyPanel->Width, propertyPanel->Height / 2);

		propertyPanel->Controls->Add(pictureBox);
	}

	void TabFunctions::GetRNAM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^)
	{
		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Type", 
			gcnew System::String("Rooms")
		}, propertyGrid);

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

		AddInfoRow(gcnew array<System::String^>(2)
		{
			"Number of Rooms",
				gcnew System::String(std::to_string(room_names.size()).c_str())
		}, propertyGrid);

		for (size_t i = 0; i < room_names.size(); i++)
			AddInfoRow(gcnew array<System::String^>(2)
			{
				gcnew System::String(std::to_string(i).c_str()), 
				gcnew System::String(room_names[i].c_str())
			}, propertyGrid);
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