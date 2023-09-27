#pragma once

#include <vector>
#include "file/FileType.h"

namespace System
{
	ref class Object;
	ref class EventArgs;
	namespace Windows
	{
		namespace Forms
		{
			ref class TabPage;
			ref class DataGridView;
			ref class Panel;
		};
	};
};

namespace HumongousFileEditor
{
	ref class HumongousNode;

	namespace chunk_reader
	{
		class FileContainer;
		struct SDAT_Chunk;
		struct HSHD_Chunk;
		struct RMHD_Chunk;
		struct IMHD_Chunk;
	}

	public ref class TabFunctions
	{
	public:
		System::Void PlayButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void StopButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ExportButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ReplaceSongButton_Click(System::Object^ sender, System::EventArgs^ e);
		void AddTab(HumongousNode^ node, System::Windows::Forms::TabControl^ tabControl);
	private:
		void GetTalk(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetSong(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetDigi(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetScrp(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetRoomBackground(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY);
		//void GetOBIM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY);
		void GetRNAM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetRoomImage(chunk_reader::FileContainer*& fc, size_t offset, size_t lflf, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY);
		void AddSoundButtons(System::Windows::Forms::TabPage^ tab, size_t offset, files::FileType fileType, System::Windows::Forms::Panel^ panel);
	};
};