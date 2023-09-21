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

	struct img_info
	{
		unsigned char* data = nullptr;
		uint16_t channels = 0;
		size_t size = 0;
		size_t width = 0, height = 0;
	};

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
		void GetRMIM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY);
		//void GetOBIM(chunk_reader::FileContainer*& fc, size_t offset, System::Windows::Forms::TabPage^ tab, System::Windows::Forms::DataGridView^ propertyGrid, System::Windows::Forms::Panel^ panel, System::Windows::Forms::Panel^ propertyPanel, float& posX, float& posY);
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
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk);
	};

	class RoomBackgroundTab
	{
	public:
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, img_info& info);
	};

	//class RoomImageTab
	//{
	//public:
	//	static bool GetData(chunk_reader::FileContainer*& fc, size_t offset, std::vector<img_info>& info);
	//};

	class ImageTab
	{
	public:
		static bool DecodeBMAP(chunk_reader::FileContainer*& fc, size_t offset, img_info& info);
		//static bool DecodeSMAP(chunk_reader::FileContainer*& fc, size_t offset, img_info& info);
		static bool DecodeHE(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info);
		static bool EncodeHE(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info);
		static bool DecodeBasic(unsigned char fill_color, unsigned char* data, size_t data_size, size_t width, size_t height, int palen, bool transparent, img_info& info);
	};
}