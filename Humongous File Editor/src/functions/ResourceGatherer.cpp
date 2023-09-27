#include "functions/ResourceGatherer.h"

#include "systems/Logger.h"
#include "lowlevel/FileContainer.h"
#include "file/FileType.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include "file/Files.h"
#include "file/Resource.h"
#include "forms/HumongousNode.h"
#include "lowlevel/HumongousChunks.h"
#include "HumongousEditorForm.h"
#include "file/ResourceType.h"
#include "lowlevel/utils.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		std::string GetCategoryName(files::ResourceType resourceType)
		{
			switch (resourceType)
			{
				case files::ResourceType::Talkie:
				{
					return "Voice Files";
				}
				case files::ResourceType::SFX:
				{
					return "SFX";
				}
				case files::ResourceType::Song:
				{
					return "Soundtracks";
				}
				case files::ResourceType::RoomBackground:
				{
					return "Room Background";
				}
				case files::ResourceType::RoomImage:
				{
					return "Room Images";
				}
				case files::ResourceType::Script:
				{
					return "Scripts";
				}
			}
			return "";
		}

		void setProgressBar(System::Windows::Forms::ToolStripProgressBar^ progressBar, int value)
		{
			value = std::clamp(value, 0, 100);
			int higherValue = value + 1;
			higherValue = std::clamp(higherValue, 0, 100);
			progressBar->Value = higherValue;
			progressBar->Value = value;
		}

		bool ResourceGatherer::Read(const char* path)
		{
			FileContainer* fc = files::FILES.Read(path);
			if (fc == nullptr)
			{
				LOGF(logger::LOGSEVERITY_ERROR, "Could not open file \"%s\".", path);
				return false;
			}

			switch (fc->fileType)
			{
				case files::FileType::FileType_HE0:
				case files::FileType::FileType_A:
				{
					return ReadResourceFile(fc);
					break;
				}
				case files::FileType::FileType_HE2:
				{
					return ReadHE2(fc);
					break;
				}
				case files::FileType::FileType_HE4:
				{
					return ReadHE4(fc);
					break;
				}
				default:
					break;
			}
			return false;
		}

		bool ResourceGatherer::ReadResourceFile(FileContainer*& fc)
		{
			FileContainer* a = nullptr;
			FileContainer* he0 = nullptr;
			if (fc->fileType == files::FileType::FileType_A)
			{
				a = fc;
				std::string he0path = files::getFileWithoutExtension(fc->path) + ".he0";

				he0 = files::FILES.Read(he0path.c_str());
				if (he0 == nullptr)
				{
					LOGF(logger::LOGSEVERITY_ERROR, "Could not open file \"%s\".", he0path.c_str());
					return false;
				}
			}
			else if (fc->fileType == files::FileType_HE0)
			{
				he0 = fc;
				std::string apath = files::getFileWithoutExtension(fc->path) + ".(a)";

				a = files::FILES.Read(apath.c_str());
				if (a == nullptr)
				{
					LOGF(logger::LOGSEVERITY_ERROR, "Could not open file \"%s\".", apath.c_str());
					return false;
				}
			}
			else
				return false;

			chunk_reader::ChunkInfo info = he0->GetChunkInfo(0);

			while (info.offset < he0->size)
			{
				if (utils::chunkcmp(info.chunk_id, chunk_reader::RNAM_CHUNK_ID) == 0)
					break;
				info = he0->GetNextChunk(info.offset);
			}
			if (utils::chunkcmp(info.chunk_id, chunk_reader::RNAM_CHUNK_ID) != 0)
				return false;

			// Get HSHD chunk for the sample rate.
			size_t rnam_offset = info.offset;
			if (rnam_offset == -1)
				return false;

			std::vector<std::string> room_names;

			chunk_reader::RNAM_Chunk* rnam_chunk = reinterpret_cast<chunk_reader::RNAM_Chunk*>(utils::add(he0->data, rnam_offset));
			size_t rnam_end = rnam_offset + rnam_chunk->ChunkSize();
			size_t pos = rnam_offset + sizeof(chunk_reader::HumongousHeader) + sizeof(uint16_t);
			std::string room_name;

			while (pos < rnam_end)
			{
				unsigned char ch;
				memcpy(&ch, utils::add(he0->data, pos), sizeof(char));
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

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
			System::Windows::Forms::TreeNode^ baseNode = form->GetBaseNode(gcnew System::String("HE0"));

			{
				HumongousNode^ node;
				node = (gcnew HumongousNode);
				node->fileType = he0->fileType;
				node->Name = gcnew System::String("ROOMS");
				node->offset = rnam_offset;
				node->Text = gcnew System::String("ROOMS");
				baseNode->Nodes->Add(node);
			}

			for (size_t i = 0; i < room_names.size(); i++)
			{
				HumongousNode^ node;
				node = (gcnew HumongousNode);
				node->fileType = he0->fileType;
				std::string unique_name = room_names[i];
				node->Name = gcnew System::String(unique_name.c_str());
				node->Text = gcnew System::String(room_names[i].c_str());
				baseNode->Nodes->Add(node);
			}

			std::map<std::string, files::ResourceType> RESOURCE_CHUNKS =
			{
				{ TALK_CHUNK_ID, files::ResourceType::Talkie },
				{ SGEN_CHUNK_ID, files::ResourceType::Song },
				{ SCRP_CHUNK_ID, files::ResourceType::Script },
				{ IM00_CHUNK_ID, files::ResourceType::RoomBackground },
				{ IM01_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM02_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM03_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM04_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM05_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM06_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM07_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM08_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM09_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM10_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM11_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM12_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM13_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM14_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM15_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM16_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM17_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM0A_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM0B_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM0C_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM0D_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM0E_CHUNK_ID, files::ResourceType::RoomImage },
				{ IM0F_CHUNK_ID, files::ResourceType::RoomImage },
				{ DIGI_CHUNK_ID, files::ResourceType::SFX }
			};

			uint32_t lflf = 0;

			ChunkInfo header = a->GetChunkInfo(0);
			int random_number_for_unique_id = 0;
			while (header.offset < a->size)
			{
				if (utils::chunkcmp(header.chunk_id, chunk_reader::LFLF_CHUNK_ID) == 0)
				{
					ChunkInfo child_header = a->GetChunkInfo(header.offset);
					uint32_t i = 0;
					while (child_header.offset < header.offset + header.ChunkSize())
					{
						std::string chunk_id_name = std::string(reinterpret_cast<char*>(child_header.chunk_id));
						chunk_id_name.resize(CHUNK_ID_SIZE);

						if (RESOURCE_CHUNKS.find(chunk_id_name) != RESOURCE_CHUNKS.end())
						{
							System::Windows::Forms::TreeNode^ lflf_node = form->GetNode(baseNode, gcnew System::String(room_names[lflf].c_str()));

							files::ResourceType type = RESOURCE_CHUNKS.at(chunk_id_name);
							System::Windows::Forms::TreeNode^ categoryNode = form->GetNode(lflf_node, gcnew System::String(GetCategoryName(type).c_str()));

							HumongousNode^ node;
							node = (gcnew HumongousNode);
							node->offset = child_header.offset;
							node->lflf = header.offset;
							node->fileType = a->fileType;
							std::string unique_name = std::string(chunk_id_name) + "_" + std::to_string(fc->fileType) + "_" + std::to_string(i) + "_" + fc->path + "_" + std::to_string(lflf) + "_" + std::to_string(random_number_for_unique_id);
							node->Name = gcnew System::String(unique_name.c_str());
							node->Text = gcnew System::String(std::to_string(i).c_str());
							categoryNode->Nodes->Add(node);
							i++;
						}
						child_header = a->GetNextChunk(child_header.offset);
						random_number_for_unique_id++;
					}

					lflf++;
				}
				header = a->GetNextChunk(header.offset);

				setProgressBar(form->toolProgressBar, 100.0f / fc->size * header.offset);
			}
			LOGF(logger::LOGSEVERITY_INFO, "Successfully gathered all .(A) and .HE0 resources for file \"%s\".", fc->path.c_str());
			return true;
		}

		bool ResourceGatherer::ReadHE2(FileContainer*& fc)
		{
			std::map<std::string, files::ResourceType> RESOURCE_CHUNKS =
			{
				{ TALK_CHUNK_ID, files::ResourceType::Talkie }
			};

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];

			std::vector<files::Resource> offsets;
			ChunkInfo header = fc->GetChunkInfo(0);
			int i = 0;
			while (header.offset < fc->size)
			{
				std::string chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
				chunk_id_name.resize(CHUNK_ID_SIZE);

				if (RESOURCE_CHUNKS.find(chunk_id_name) != RESOURCE_CHUNKS.end())
				{
					files::Resource resource;
					resource.offset = header.offset;
					resource.name = std::to_string(i);
					resource.type = RESOURCE_CHUNKS.at(chunk_id_name);
					offsets.push_back(resource);
					i++;
				}
				header = fc->GetNextChunk(header.offset);

				setProgressBar(form->toolProgressBar, 100.0f / fc->size * header.offset);
			}

			System::Windows::Forms::TreeNode^ baseNode = form->GetBaseNode(gcnew System::String("HE2"));

			for (size_t i = 0; i < offsets.size(); i++)
			{
				System::Windows::Forms::TreeNode^ categoryNode = form->GetNode(baseNode, gcnew System::String(GetCategoryName(offsets[i].type).c_str()));
				HumongousNode^ node;
				node = (gcnew HumongousNode);
				node->offset = offsets[i].offset;
				node->fileType = fc->fileType;
				node->Name = gcnew System::String(offsets[i].name.c_str());
				node->Text = gcnew System::String(offsets[i].name.c_str());
				categoryNode->Nodes->Add(node);
			}
			LOGF(logger::LOGSEVERITY_INFO, "Successfully gathered all .HE2 resources for file \"%s\".", fc->path.c_str());
			return true;
		}

		bool ResourceGatherer::ReadHE4(FileContainer*& fc)
		{
			std::map<std::string, files::ResourceType> RESOURCE_CHUNKS =
			{
				{ SGEN_CHUNK_ID, files::ResourceType::Song }
			};

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];

			std::vector<files::Resource> offsets;
			ChunkInfo header = fc->GetChunkInfo(0);
			int i = 0;
			while (header.offset < fc->size)
			{
				std::string chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
				chunk_id_name.resize(CHUNK_ID_SIZE);

				if (RESOURCE_CHUNKS.find(chunk_id_name) != RESOURCE_CHUNKS.end())
				{
					files::Resource resource;
					resource.offset = header.offset;
					resource.name = std::to_string(i);
					resource.type = RESOURCE_CHUNKS.at(chunk_id_name);
					offsets.push_back(resource);
					i++;
				}
				header = fc->GetNextChunk(header.offset);

				setProgressBar(form->toolProgressBar, 100.0f / fc->size * header.offset);
			}

			System::Windows::Forms::TreeNode^ baseNode = form->GetBaseNode(gcnew System::String("HE4"));

			for (size_t i = 0; i < offsets.size(); i++)
			{
				System::Windows::Forms::TreeNode^ categoryNode = form->GetNode(baseNode, gcnew System::String(GetCategoryName(offsets[i].type).c_str()));
				HumongousNode^ node;
				node = (gcnew HumongousNode);
				node->offset = offsets[i].offset;
				node->fileType = fc->fileType;
				node->Name = gcnew System::String(offsets[i].name.c_str()) + gcnew System::String(std::to_string(i).c_str());
				node->Text = gcnew System::String(offsets[i].name.c_str());
				categoryNode->Nodes->Add(node);
			}
			LOGF(logger::LOGSEVERITY_INFO, "Successfully gathered all .HE4 resources for file \"%s\".", fc->path.c_str());
			return true;
		}
	}
}