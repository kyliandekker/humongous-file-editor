#include "functions/ResourceGatherer.h"

#include "lowlevel/FileContainer.h"
#include "file/FileType.h"
#include "lowlevel/HumongousChunkDefinitions.h"
#include "file/Files.h"
#include "file/Resource.h"
#include "forms/HumongousNode.h"
#include "HumongousEditorForm.h"
#include "file/ResourceType.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		void ResourceGatherer::Read(const char* path)
		{
			FileContainer* fc = files::FILES.Read(path);
			if (fc == nullptr)
			{
				System::Windows::Forms::MessageBox::Show("Cannot open file.", "Decompilation failed", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
				return;
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
			};

			std::string fileExt = "";
			switch (fc->fileType)
			{
				case files::FileType_A:
				{
					fileExt = "(a)";
					RESOURCE_CHUNKS.insert({ DIGI_CHUNK_ID, files::ResourceType::SFX });
					break;
				}
				case files::FileType_HE2:
				{
					fileExt = "HE2";
					break;
				}
				case files::FileType_HE4:
				{
					fileExt = "HE4";
					break;
				}
			}

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
			System::Windows::Forms::TreeNode^ baseNode = form->GetBaseNode(gcnew System::String(fileExt.c_str()));

			for (size_t i = 0; i < offsets.size(); i++)
			{
				System::Windows::Forms::TreeNode^ parentNode;
				switch (offsets[i].type)
				{
					case files::ResourceType::Talkie:
					{
						parentNode = form->GetNode(baseNode, gcnew System::String("Voice Files"));
						break;
					}
					case files::ResourceType::SFX:
					{
						parentNode = form->GetNode(baseNode, gcnew System::String("SFX"));
						break;
					}
					case files::ResourceType::Song:
					{
						parentNode = form->GetNode(baseNode, gcnew System::String("Soundtracks"));
						break;
					}
					case files::ResourceType::Image:
					{
						parentNode = form->GetNode(baseNode, gcnew System::String("Images"));
						break;
					}
					case files::ResourceType::Script:
					{
						parentNode = form->GetNode(baseNode, gcnew System::String("Scripts"));
						break;
					}
				}
				HumongousNode^ node;
				node = (gcnew HumongousNode);
				node->offset = offsets[i].offset;
				node->fileType = fc->fileType;
				node->Name = gcnew System::String(offsets[i].name.c_str());
				node->Text = gcnew System::String(offsets[i].name.c_str());
				parentNode->Nodes->Add(node);
			}
		}
	}
}