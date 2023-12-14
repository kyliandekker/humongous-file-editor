#include "game/compilers/ResourceFileCompiler.h"

#include <map>
#include <string>

#include "low_level/HumongousChunkDefinitions.h"
#include "game/GameResourceType.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "low_level/ChunkInfo.h"
#include "system/Logger.h"
#include "utils/string.h"
#include "low_level/Utils.h"
#include "low_level/HumongousChunks.h"

namespace resource_editor
{
	namespace game
	{
		struct version_key
		{
			std::string ext = 0;
			uint32_t size = 0;

			bool operator <(const version_key& rhs) const
			{
				return size < rhs.size;
			}
		};

		struct version_value
		{
			uint8_t version = 0;
			uint8_t he_version = 0;
		};

		bool ResourceFileCompiler::Decompile(project::Resource& a_Resource, std::vector<GameResource>& a_Resources)
		{
			if (!a_Resource.m_Parent)
			{
				return false;
			}

			project::Resource* he0 = nullptr;
			project::Resource* a = nullptr;
			for (size_t i = 0; i < a_Resource.m_Parent->m_Resources.size(); i++)
			{
				project::Resource& resource = a_Resource.m_Parent->m_Resources[i];
				if (string_extensions::getFileWithoutExtension(resource.m_Name).compare(string_extensions::getFileWithoutExtension(a_Resource.m_Name)) == 0 && resource.m_ResourceType == project::ResourceType::A)
				{
					a = &resource;
				}
				else if (string_extensions::getFileWithoutExtension(resource.m_Name).compare(string_extensions::getFileWithoutExtension(a_Resource.m_Name)) == 0 && resource.m_ResourceType == project::ResourceType::HE0)
				{
					he0 = &resource;
				}
			}

			if (!a)
			{
				return false;
			}

			if (!he0)
			{
				return false;
			}

			// STEP 1. Read version.
			std::map<version_key, version_value> versions =
			{
				{ { ".LA0", 138 }, { 7, 0 } },
				{ { ".LA0", 176 }, { 8, 0 } },
				{ { ".000", 138 }, { 8, 0 } },
				{ { ".HE0", 52 }, { 6, 99 } },
				{ { ".HE0", 46 }, { 6, 90 } },
				{ { ".HE0", 40 }, { 6, 80 } },
				{ { ".HE0", 38 }, { 6, 71 } },
				{ { ".HE0", 38 }, { 6, 71 } },
				{ { ".000", 38 }, { 6, 0 } },
				{ { ".SM0", 38 }, { 6, 0 } },
				{ { ".000", 26 }, { 5, 0 } },
				{ { ".LFL", 26 }, { 5, 0 } }
			};

			version_key key = { string_extensions::getExtensionFromPath(he0->m_Path), he0->m_FileContainer.size };
			version_value values = versions[key];

			LOGF(logger::LOGSEVERITY_INFO, "Index file has version %i and he version %i.", values.version, values.he_version);

			chunk_reader::ChunkInfo info = he0->m_FileContainer.GetChunkInfo(0);

			while (info.offset < he0->m_FileContainer.size)
			{
				if (utils::chunkcmp(info.chunk_id, chunk_reader::RNAM_CHUNK_ID) == 0)
				{
					break;
				}
				info = he0->m_FileContainer.GetNextChunk(info.offset);
			}
			if (utils::chunkcmp(info.chunk_id, chunk_reader::RNAM_CHUNK_ID) != 0)
			{
				return false;
			}

			size_t rnam_offset = info.offset;
			if (rnam_offset == -1)
			{
				return false;
			}

			std::vector<std::string> room_names;

			chunk_reader::RNAM_Chunk* rnam_chunk = reinterpret_cast<chunk_reader::RNAM_Chunk*>(utils::add(he0->data, rnam_offset));
			size_t rnam_end = rnam_offset + rnam_chunk->ChunkSize();
			size_t pos = rnam_offset + sizeof(chunk_reader::HumongousHeader) + sizeof(uint16_t);
			std::string room_name;

			while (pos < rnam_end)
			{
				unsigned char ch;
				memcpy(&ch, utils::add(he0->m_FileContainer.data, pos), sizeof(char));
				if (utils::unsignedCharCmp(ch, '\0'))
				{
					room_names.push_back(room_name);
					room_name = "";
					pos += sizeof(uint16_t);
				}
				else
				{
					room_name += ch;
				}
				pos++;
			}

			for (size_t i = 0; i < room_names.size(); i++)
			{
				GameResource resource;
				resource.name = room_names[i];
				resource.offset = rnam_offset;
				resource.type = game::GameResourceType::Room;
				a_Resources.push_back(resource);
			}

			std::map<std::string, game::GameResourceType> RESOURCE_CHUNKS =
			{
				{ chunk_reader::TALK_CHUNK_ID, game::GameResourceType::Talkie },
				{ chunk_reader::SGEN_CHUNK_ID, game::GameResourceType::Song },
				{ chunk_reader::SCRP_CHUNK_ID, game::GameResourceType::Global_Script },
				{ chunk_reader::LSCR_CHUNK_ID, game::GameResourceType::Local_Script },
				{ chunk_reader::LSC2_CHUNK_ID, game::GameResourceType::Local_Script },
				{ chunk_reader::VERB_CHUNK_ID, game::GameResourceType::Verb_Script },
				//chunk_reader::{ ENCD_CHUNK_IDgameesGame::ResourceType::Global_Script },
				//chunk_reader::{ EXCD_CHUNK_IDgameesGame::ResourceType::Global_Script },
				{ chunk_reader::IM00_CHUNK_ID, game::GameResourceType::RoomBackground },
				{ chunk_reader::IM01_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM02_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM03_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM04_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM05_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM06_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM07_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM08_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM09_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM10_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM11_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM12_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM13_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM14_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM15_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM16_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM17_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM0A_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM0B_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM0C_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM0D_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM0E_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::IM0F_CHUNK_ID, game::GameResourceType::RoomImage },
				{ chunk_reader::DIGI_CHUNK_ID, game::GameResourceType::SFX }
			};

			uint32_t lflf = 0;

			chunk_reader::ChunkInfo header = a->m_FileContainer.GetChunkInfo(0);
			int random_number_for_unique_id = 0;
			while (header.offset < a->m_FileContainer.size)
			{
				if (utils::chunkcmp(header.chunk_id, chunk_reader::LFLF_CHUNK_ID) == 0)
				{
					chunk_reader::ChunkInfo child_header = a->m_FileContainer.GetChunkInfo(header.offset);
					uint32_t i = 0;
					while (child_header.offset < header.offset + header.ChunkSize())
					{
						std::string chunk_id_name = std::string(reinterpret_cast<char*>(child_header.chunk_id));
						chunk_id_name.resize(CHUNK_ID_SIZE);

						std::map<std::string, game::GameResourceType>::iterator it = RESOURCE_CHUNKS.find(chunk_id_name);
						if (it != RESOURCE_CHUNKS.end())
						{
							System::Windows::Forms::TreeNode^ lflf_node = form->GetNode(baseNode, gcnew System::String(room_names[lflf].c_str()));

							files::ResourceType type = RESOURCE_CHUNKS.at(chunk_id_name);
							System::Windows::Forms::TreeNode^ categoryNode = form->GetNode(lflf_node, gcnew System::String(GetCategoryName(type).c_str()));

							HumongousNode^ node;
							node = (gcnew HumongousNode);
							node->offset = child_header.offset;
							node->fileType = a->fileType;
							node->resourceType = it->second;
							std::string unique_name = std::string(chunk_id_name) + "_" + std::to_string(fc->fileType) + "_" + std::to_string(i) + "_" + fc->path + "_" + std::to_string(lflf) + "_" + std::to_string(random_number_for_unique_id);
							node->Name = gcnew System::String(unique_name.c_str());
							node->Text = gcnew System::String(std::to_string(i).c_str());
							categoryNode->Nodes->Add(node);
							if (node->resourceType == files::ResourceType::RoomImage)
							{
								HumongousNode^ childNode;
								childNode = (gcnew HumongousNode);
								childNode->offset = child_header.offset;
								childNode->fileType = a->fileType;
								childNode->special = true;
								childNode->resourceType = it->second;
								std::string unique_name = std::string(chunk_id_name) + "_" + std::to_string(fc->fileType) + "_" + std::to_string(i) + "_" + fc->path + "_" + std::to_string(lflf) + "_" + std::to_string(random_number_for_unique_id) + "_special_layer";
								childNode->Name = gcnew System::String(unique_name.c_str());
								childNode->Text = gcnew System::String(std::string(std::to_string(i) + " layer").c_str());
								node->Nodes->Add(childNode);
							}
							if (node->resourceType == files::ResourceType::RoomBackground)
							{
								HumongousNode^ childNode;
								childNode = (gcnew HumongousNode);
								childNode->offset = child_header.offset;
								childNode->fileType = a->fileType;
								childNode->special = true;
								childNode->resourceType = it->second;
								std::string unique_name = std::string(chunk_id_name) + "_" + std::to_string(fc->fileType) + "_" + std::to_string(i) + "_" + fc->path + "_" + std::to_string(lflf) + "_" + std::to_string(random_number_for_unique_id) + "_special_palette";
								childNode->Name = gcnew System::String(unique_name.c_str());
								childNode->Text = gcnew System::String(std::string(std::to_string(i) + " palette").c_str());
								node->Nodes->Add(childNode);
							}
							i++;
						}
						child_header = a->GetNextChunk(child_header.offset);
						random_number_for_unique_id++;

						ChunkFunctions::SetGlobalProgressBar("Reading resource file", static_cast<double>(100.0f / fc->size * header.offset));
						ChunkFunctions::SetProgressBar("Reading room chunk childs", static_cast<double>(100.0f / (header.offset + header.ChunkSize()) * child_header.offset));
					}

					lflf++;
				}

				header = a->GetNextChunk(header.offset);
			}
			ChunkFunctions::SetGlobalProgressBar("Reading resource file", 100.0f);
			LOGF(logger::LOGSEVERITY_INFO, "Successfully gathered all .(A) and .HE0 resources for file \"%s\".", fc->path.c_str());
			return true;
		}
	}
}