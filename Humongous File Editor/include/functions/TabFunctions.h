#pragma once

#include "forms/HumongousNode.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;
		class SDAT_Chunk;
		class HSHD_Chunk;
	}

	public ref class TabFunctions
	{
	public:
		System::Void PlayButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void StopButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ExportButton_Click(System::Object^ sender, System::EventArgs^ e);
		void AddTab(HumongousNode^ node, System::Windows::Forms::TabControl^ tabControl);
	private:
		void GetTalk(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetSong(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetDigi(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetScrp(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetImXX(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void GetRNAM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, float& posX, float& posY);
		void AddSoundButtons(System::Windows::Forms::TabPage^ tab, size_t offset, files::FileType fileType, System::Windows::Forms::Panel^ panel);
	};

	class TalkieTab
	{
	public:
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk);
	};

	class SongTab
	{
	public:
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk);
	};

	class DigiTab
	{
	public:
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offse, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunkt);
	};
}