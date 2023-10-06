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
			ref class MouseEventArgs;
			ref class Void;
		};
	};
};

namespace HumongousFileEditor
{
	ref class HumongousNode;

	struct img_info;

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
		System::Void NextButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ExportButton_Click(System::Object^ sender, System::EventArgs^ e);

		System::Void ExportPaletteButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ExportBmapButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ImportPaletteButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ImportBmapButton_Click(System::Object^ sender, System::EventArgs^ e);

		System::Void ReplaceButton_Click(System::Object^ sender, System::EventArgs^ e);
		void AddTab(HumongousNode^ node, System::Windows::Forms::TabControl^ tabControl);
		System::Void DoubleClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
	private:
		void GetTalk(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetSong(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetDigi(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetGlobalScript(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetLocalScript(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		bool GetRoomBackgroundData(chunk_reader::FileContainer*& fc, size_t offset, img_info& info);
		void GetRoomBackground(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY);
		void GetRNAM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		bool GetRoomImageData(chunk_reader::FileContainer*& fc, size_t offset, img_info& info);
		void GetRoomImage(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY);
		bool GetRoomImageLayerData(chunk_reader::FileContainer*& fc, size_t offset, img_info& info);
		void GetRoomImageLayer(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY);
		void AddSoundButtons(System::Windows::Forms::TabPage^ tab, size_t offset, files::FileType fileType, System::Windows::Forms::Panel^ panel);
	};
};