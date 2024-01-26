#include "game/compilers/ResourceFileCompiler.h"

#include <map>
#include <string>

#include "low_level/HumongousChunkDefinitions.h"
#include "game/GameResourceType.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "project/Project.h"
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
			size_t size = 0;

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

			project::Resource* he0 = project::project.m_LoadedResources[(int)project::ResourceType::HE0];
			project::Resource* a = project::project.m_LoadedResources[(int)project::ResourceType::A];

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

			const version_key key = { string_extensions::getExtensionFromPath(he0->m_Path), he0->m_FileContainer.size()};
			const version_value values = versions[key];

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

			std::vector<chunk_reader::ChunkInfo> desired = { chunk_reader::ChunkInfo(chunk_reader::RNAM_CHUNK_ID) };

			if (low_level::utils::seekChildren(*he0, he0->m_FileContainer.size(), desired) < desired.size())
			{
				return false;
			}

			// Extract rooms.
			std::vector<std::string> room_names;

			chunk_reader::RNAM_Chunk rnam_chunk;
			he0->m_FileContainer.GetChunk(rnam_chunk, desired[0].m_Offset);
			rnam_chunk.data = low_level::utils::add(he0->m_FileContainer.data(), desired[0].m_Offset + sizeof(chunk_reader::HumongousHeader));
			
			const size_t rnam_end = desired[0].m_Offset + rnam_chunk.ChunkSize();
			size_t pos = desired[0].m_Offset + sizeof(chunk_reader::HumongousHeader) + sizeof(uint16_t);
			std::string room_name;

			while (pos < rnam_end)
			{
				unsigned char ch;
				memcpy(&ch, low_level::utils::add(he0->m_FileContainer.data(), pos), sizeof(char));
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

			if (room_names.size() == 0)
			{
				return false;
			}

			// Add rooms.
			for (std::string& room_name : room_names)
			{
				GameResource resource;
				resource.m_Name = room_name;
				resource.m_Offset = desired[0].m_Offset;
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
				{ chunk_reader::ENCD_CHUNK_ID, game::GameResourceType::Global_Script },
				{ chunk_reader::EXCD_CHUNK_ID, game::GameResourceType::Global_Script },
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
			while (header.m_Offset < a->m_FileContainer.size())
			{
				if (low_level::utils::chunkcmp(header.chunk_id, chunk_reader::LFLF_CHUNK_ID) == 0)
				{
					chunk_reader::ChunkInfo child_header = a->m_FileContainer.GetChunkInfo(header.m_Offset);
					uint32_t i = 0;
					while (child_header.m_Offset < header.m_Offset + static_cast<int32_t>(header.ChunkSize()))
					{
						std::string chunk_id_name = std::string(reinterpret_cast<char*>(child_header.chunk_id));
						chunk_id_name.resize(CHUNK_ID_SIZE);

						const std::map<std::string, game::GameResourceType>::iterator it = RESOURCE_CHUNKS.find(chunk_id_name);
						if (it != RESOURCE_CHUNKS.end())
						{
							GameResource resource;
							resource.m_Offset = child_header.m_Offset;
							resource.m_Type = it->second;
							resource.m_Name = std::to_string(i);
							resource.m_Parent = a;
							resource.m_Properties =
							{
								{ "1. Name", resource.m_Name },
								{ "2. Offset", std::to_string(resource.m_Offset) },
								{ "3. Type", getResourceTypeName(resource.m_Type) },
								{ "4. Room", std::to_string(lflf) },
							};
							if (resource.m_Type == game::GameResourceType::RoomImage || resource.m_Type == game::GameResourceType::RoomBackground)
							{
								GameResource child_resource = resource;
								child_resource.m_Parent = a;
								if (resource.m_Type == game::GameResourceType::RoomImage)
								{
									child_resource.m_Name = resource.m_Name + " placement" + getExtension(resource.m_Type);
									child_resource.m_Type = RoomImageLayer;
									child_resource.m_Properties["1. Name"] = child_resource.m_Name;
									child_resource.m_Properties["3. Type"] = getResourceTypeName(child_resource.m_Type);
									resource.m_Resources.push_back(child_resource);
								}
							}
							resource.m_Name += getExtension(resource.m_Type);
							resource.m_Properties["1. Name"] = resource.m_Name;
							a_Resources[lflf].m_Resources.push_back(resource);
							i++;
						}
						child_header = a->m_FileContainer.GetNextChunk(child_header.m_Offset);
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