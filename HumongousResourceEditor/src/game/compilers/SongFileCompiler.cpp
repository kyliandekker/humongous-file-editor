#include "game/compilers/SongFileCompiler.h"

#include <map>
#include <string>

#include "low_level/HumongousChunkDefinitions.h"
#include "game/GameResourceType.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "low_level/ChunkInfo.h"
#include "system/Logger.h"

namespace resource_editor
{
	namespace game
	{
		bool SongFileCompiler::Decompile(project::Resource& a_Resource, std::vector<GameResource>& a_Resources)
		{
			std::map<std::string, game::GameResourceType> RESOURCE_CHUNKS =
			{
				{ chunk_reader::SGEN_CHUNK_ID, game::GameResourceType::Song }
			};

			chunk_reader::ChunkInfo header = a_Resource.m_FileContainer.GetChunkInfo(0);
			int i = 0;
			while (header.m_Offset < a_Resource.m_FileContainer.m_Size)
			{
				std::string chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
				chunk_id_name.resize(CHUNK_ID_SIZE);

				if (RESOURCE_CHUNKS.find(chunk_id_name) != RESOURCE_CHUNKS.end())
				{
					game::GameResource resource;
					resource.m_Offset = header.m_Offset;
					resource.m_Type = RESOURCE_CHUNKS.at(chunk_id_name);
					resource.m_Name = std::to_string(i) + getExtension(resource.m_Type);
					resource.m_Parent = &a_Resource;
					resource.m_Properties =
					{
						{ "Offset", std::to_string(resource.m_Offset) },
						{ "Type", getResourceTypeName(resource.m_Type) },
						{ "Name", resource.m_Name },
					};
					a_Resources.push_back(resource);
					i++;
				}
				header = a_Resource.m_FileContainer.GetNextChunk(header.m_Offset);
			}

			LOGF(logger::LOGSEVERITY_INFO, "Successfully gathered all .HE4 resources for file \"%s\".", a_Resource.m_FileContainer.m_Path.c_str());
			return true;
		}
	}
}