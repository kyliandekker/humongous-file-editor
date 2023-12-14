#include "game/compilers/TalkFileCompiler.h"

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
		bool TalkFileCompiler::Decompile(project::Resource& a_Resource, std::vector<GameResource>& a_Resources)
		{
			std::map<std::string, game::GameResourceType> RESOURCE_CHUNKS =
			{
				{ chunk_reader::TALK_CHUNK_ID, game::GameResourceType::Talkie }
			};

			chunk_reader::ChunkInfo header = a_Resource.m_FileContainer.GetChunkInfo(0);
			int i = 0;
			while (header.offset < a_Resource.m_FileContainer.size)
			{
				std::string chunk_id_name = std::string(reinterpret_cast<char*>(header.chunk_id));
				chunk_id_name.resize(CHUNK_ID_SIZE);

				if (RESOURCE_CHUNKS.find(chunk_id_name) != RESOURCE_CHUNKS.end())
				{
					game::GameResource resource;
					resource.offset = header.offset;
					resource.name = std::to_string(i);
					resource.type = RESOURCE_CHUNKS.at(chunk_id_name);
					a_Resources.push_back(resource);
					i++;
				}
				header = a_Resource.m_FileContainer.GetNextChunk(header.offset);
			}

			LOGF(logger::LOGSEVERITY_INFO, "Successfully gathered all .HE2 resources for file \"%s\".", a_Resource.m_FileContainer.path.c_str());
			return true;
		}
	}
}