#include "functions/ResourceGatherer.h"

#include "lowlevel/FileContainer.h"
#include "file/FileType.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include "file/Files.h"
#include "file/Resource.h"
#include "forms/HumongousNode.h"
#include "lowlevel/HumongousChunks.h"
#include "HumongousEditorForm.h"
#include "file/ResourceType.h"

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
				case files::ResourceType::Image:
				{
					return "Images";
				}
				case files::ResourceType::Script:
				{
					return "Scripts";
				}
			}
			return "";
		}

		void ResourceGatherer::Read(const char* path)
		{
			FileContainer* fc = files::FILES.Read(path);
			if (fc == nullptr)
			{
				System::Windows::Forms::MessageBox::Show("Cannot open file.", "Decompilation failed", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
				return;
			}

			switch (fc->fileType)
			{
				case files::FileType::FileType_HE0:
				case files::FileType::FileType_A:
				{
					ReadResourceFile(fc);
					break;
				}
				case files::FileType::FileType_HE2:
				{
					ReadHE2(fc);
					break;
				}
				case files::FileType::FileType_HE4:
				{
					ReadHE4(fc);
					break;
				}
				default:
				{
					break;
				}
			}
		}

		void ResourceGatherer::ReadResourceFile(FileContainer*& fc)
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
					System::Windows::Forms::MessageBox::Show("Cannot open file.", "Decompilation failed", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
					return;
				}
			}
			else if (fc->fileType == files::FileType_HE0)
			{
				he0 = fc;
				std::string apath = files::getFileWithoutExtension(fc->path) + ".(a)";

				a = files::FILES.Read(apath.c_str());
				if (a == nullptr)
				{
					System::Windows::Forms::MessageBox::Show("Cannot open file.", "Decompilation failed", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
					return;
				}
			}
			else
				return;

			chunk_reader::ChunkInfo info = he0->GetChunkInfo(0);

			while (info.offset < he0->size)
			{
				if (utils::chunkcmp(info.chunk_id, chunk_reader::RNAM_CHUNK_ID) == 0)
					break;
				info = he0->GetNextChunk(info.offset);
			}
			if (utils::chunkcmp(info.chunk_id, chunk_reader::RNAM_CHUNK_ID) != 0)
				return;

			// Get HSHD chunk for the sample rate.
			int32_t rnam_offset = info.offset;
			if (rnam_offset == -1)
				return;

			chunk_reader::RNAM_Chunk* rnam_chunk = reinterpret_cast<chunk_reader::RNAM_Chunk*>(utils::add(he0->data, rnam_offset));

			std::vector<std::string> room_names;

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

			for (size_t i = 0; i < room_names.size(); i++)
			{
				HumongousNode^ node;
				node = (gcnew HumongousNode);
				node->fileType = he0->fileType;
				node->Name = gcnew System::String(room_names[i].c_str());
				node->Text = gcnew System::String(room_names[i].c_str());
				baseNode->Nodes->Add(node);
			}

			std::map<std::string, files::ResourceType> RESOURCE_CHUNKS =
			{
				{ TALK_CHUNK_ID, files::ResourceType::Talkie },
				{ SGEN_CHUNK_ID, files::ResourceType::Song },
				{ SCRP_CHUNK_ID, files::ResourceType::Script },
				{ IM00_CHUNK_ID, files::ResourceType::Image },
				{ IM01_CHUNK_ID, files::ResourceType::Image },
				{ IM02_CHUNK_ID, files::ResourceType::Image },
				{ IM03_CHUNK_ID, files::ResourceType::Image },
				{ IM04_CHUNK_ID, files::ResourceType::Image },
				{ IM05_CHUNK_ID, files::ResourceType::Image },
				{ IM06_CHUNK_ID, files::ResourceType::Image },
				{ IM07_CHUNK_ID, files::ResourceType::Image },
				{ IM08_CHUNK_ID, files::ResourceType::Image },
				{ IM09_CHUNK_ID, files::ResourceType::Image },
				{ IM10_CHUNK_ID, files::ResourceType::Image },
				{ IM11_CHUNK_ID, files::ResourceType::Image },
				{ IM12_CHUNK_ID, files::ResourceType::Image },
				{ IM13_CHUNK_ID, files::ResourceType::Image },
				{ IM14_CHUNK_ID, files::ResourceType::Image },
				{ IM15_CHUNK_ID, files::ResourceType::Image },
				{ IM16_CHUNK_ID, files::ResourceType::Image },
				{ IM17_CHUNK_ID, files::ResourceType::Image },
				{ IM0A_CHUNK_ID, files::ResourceType::Image },
				{ IM0B_CHUNK_ID, files::ResourceType::Image },
				{ IM0C_CHUNK_ID, files::ResourceType::Image },
				{ IM0D_CHUNK_ID, files::ResourceType::Image },
				{ IM0E_CHUNK_ID, files::ResourceType::Image },
				{ IM0F_CHUNK_ID, files::ResourceType::Image },
				{ DIGI_CHUNK_ID, files::ResourceType::SFX }
			};

			uint32_t lflf = 0;

			ChunkInfo header = a->GetChunkInfo(0);
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
							node->fileType = a->fileType;
							node->Name = gcnew System::String(std::to_string(i).c_str());
							node->Text = gcnew System::String(std::to_string(i).c_str());
							categoryNode->Nodes->Add(node);
							i++;
						}
						child_header = a->GetNextChunk(child_header.offset);
					}

					lflf++;
				}
				header = a->GetNextChunk(header.offset);
			}

			printf("Test\n");
		}

		void ResourceGatherer::ReadHE2(FileContainer*& fc)
		{
			std::map<std::string, files::ResourceType> RESOURCE_CHUNKS =
			{
				{ TALK_CHUNK_ID, files::ResourceType::Talkie }
			};

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
			}

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
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
		}

		void ResourceGatherer::ReadHE4(FileContainer*& fc)
		{
			std::map<std::string, files::ResourceType> RESOURCE_CHUNKS =
			{
				{ SGEN_CHUNK_ID, files::ResourceType::Song }
			};

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
			}

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
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
		}
	}
}