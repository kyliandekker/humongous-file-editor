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
#include "imgui/tools/ExplorerWindow.h"

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

			project::Resource* he0 = imgui::explorer.m_LoadedResources[(int)project::ResourceType::HE0];
			project::Resource* a = imgui::explorer.m_LoadedResources[(int)project::ResourceType::A];

			// If the a is not found, return false.
			if (!a)
			{
				return false;
			}

			// If the he0 is not found, return false.
			if (!he0)
			{
				return false;
			}

			// Read the file version.
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

			version_key key = { string_extensions::getExtensionFromPath(he0->m_Path), he0->m_FileContainer.m_Size };
			version_value values = versions[key];

			LOGF(logger::LOGSEVERITY_INFO, "Index file has version %i and he version %i.", values.version, values.he_version);

			// Check header info of he0 and decrypt if necessary.
			chunk_reader::ChunkInfo info = he0->m_FileContainer.GetChunkInfo(0);

			// If after decryption it still does not start with a HE0 chunk, return and give an error.
			if (low_level::utils::chunkcmp(info.chunk_id, chunk_reader::MAXS_CHUNK_ID) != 0)
			{
				// If the first check fails, then decrypt. Maybe we already have a decrypted file, in which case it'd succeed.
				he0->m_FileContainer.Decrypt(0x69);
				info = he0->m_FileContainer.GetChunkInfo(0);

				if (low_level::utils::chunkcmp(info.chunk_id, chunk_reader::MAXS_CHUNK_ID) != 0)
				{
					return false;
				}
			}

			if (!a->m_FileContainer.Open(a->m_Path))
			{
				return false;
			}

			// Check header info of a and decrypt if necessary.
			info = a->m_FileContainer.GetChunkInfo(0);

			// If after decryption it still does not start with a (A) chunk, return and give an error.
			if (low_level::utils::chunkcmp(info.chunk_id, chunk_reader::LECF_CHUNK_ID) != 0)
			{
				// If the first check fails, then decrypt. Maybe we already have a decrypted file, in which case it'd succeed.
				a->m_FileContainer.Decrypt(0x69);
				info = a->m_FileContainer.GetChunkInfo(0);

				if (low_level::utils::chunkcmp(info.chunk_id, chunk_reader::LECF_CHUNK_ID) != 0)
				{
					return false;
				}
			}

			// Look for RNAM chunk.
			while (info.m_Offset < he0->m_FileContainer.m_Size)
			{
				if (low_level::utils::chunkcmp(info.chunk_id, chunk_reader::RNAM_CHUNK_ID) == 0)
				{
					break;
				}
				info = he0->m_FileContainer.GetNextChunk(info.m_Offset);
			}
			if (low_level::utils::chunkcmp(info.chunk_id, chunk_reader::RNAM_CHUNK_ID) != 0)
			{
				return false;
			}

			size_t rnam_offset = info.m_Offset;
			if (rnam_offset == -1)
			{
				return false;
			}

			// Extract rooms.
			std::vector<std::string> room_names;

			chunk_reader::RNAM_Chunk* rnam_chunk = reinterpret_cast<chunk_reader::RNAM_Chunk*>(low_level::utils::add(he0->m_FileContainer.m_Data, rnam_offset));
			size_t rnam_end = rnam_offset + rnam_chunk->ChunkSize();
			size_t pos = rnam_offset + sizeof(chunk_reader::HumongousHeader) + sizeof(uint16_t);
			std::string room_name;

			while (pos < rnam_end)
			{
				unsigned char ch;
				memcpy(&ch, low_level::utils::add(he0->m_FileContainer.m_Data, pos), sizeof(char));
				if (low_level::utils::unsignedCharCmp(ch, '\0'))
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

			// Add rooms.
			for (size_t i = 0; i < room_names.size(); i++)
			{
				GameResource resource;
				resource.m_Name = room_names[i];
				resource.m_Offset = rnam_offset;
				resource.m_Type = game::GameResourceType::Room;
				resource.m_Parent = he0;
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
			while (header.m_Offset < a->m_FileContainer.m_Size)
			{
				if (low_level::utils::chunkcmp(header.chunk_id, chunk_reader::LFLF_CHUNK_ID) == 0)
				{
					chunk_reader::ChunkInfo child_header = a->m_FileContainer.GetChunkInfo(header.m_Offset);
					uint32_t i = 0;
					while (child_header.m_Offset < header.m_Offset + header.ChunkSize())
					{
						std::string chunk_id_name = std::string(reinterpret_cast<char*>(child_header.chunk_id));
						chunk_id_name.resize(CHUNK_ID_SIZE);

						std::map<std::string, game::GameResourceType>::iterator it = RESOURCE_CHUNKS.find(chunk_id_name);
						if (it != RESOURCE_CHUNKS.end())
						{
							GameResource resource;
							resource.m_Offset = child_header.m_Offset;
							resource.m_Type = it->second;
							resource.m_Name = std::to_string(i);
							resource.m_Parent = a;
							if (resource.m_Type == game::GameResourceType::RoomImage || resource.m_Type == game::GameResourceType::RoomBackground)
							{
								GameResource child_resource;
								child_resource.m_Offset = resource.m_Offset;
								child_resource.m_Parent = a;
								if (resource.m_Type == game::GameResourceType::RoomImage)
								{
									child_resource.m_Name = resource.m_Name + " placement" + getExtension(resource.m_Type);
									child_resource.m_Type = RoomImage_Layer;
								}
								else if (resource.m_Type == game::GameResourceType::RoomBackground)
								{
									child_resource.m_Name = resource.m_Name + " palette" + getExtension(resource.m_Type);
									child_resource.m_Type = RoomBackground_Palette;
								}
								resource.m_Resources.push_back(child_resource);
							}
							resource.m_Name += getExtension(resource.m_Type);
							a_Resources[lflf].m_Resources.push_back(resource);
							i++;
						}
						child_header = a->m_FileContainer.GetNextChunk(child_header.m_Offset);
						random_number_for_unique_id++;
					}

					lflf++;
				}

				header = a->m_FileContainer.GetNextChunk(header.m_Offset);
			}
			LOGF(logger::LOGSEVERITY_INFO, "Successfully gathered all .(A) and .HE0 resources for file \"%s\".", a_Resource.m_Path.c_str());
			return true;
		}
	}
}