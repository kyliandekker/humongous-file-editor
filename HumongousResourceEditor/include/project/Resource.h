#pragma once

#include <vector>
#include <string>

#include "project/ResourceType.h"
#include "low_level/FileContainer.h"
#include "game/GameResource.h"

namespace resource_editor
{
	namespace project
	{
		class Resource
		{
		public:
			std::vector<Resource> m_Resources;
			std::string m_Path;
			std::string m_Name;
			bool m_FoldedOut = false;
			bool m_Show = true;
			ResourceType m_ResourceType = ResourceType::None;
			bool m_HasChildren = false;
			chunk_reader::FileContainer m_FileContainer;
			bool m_OpenedWindow = false;
			Resource* m_Parent = nullptr;
			std::vector<game::GameResource> m_GameResources;

			void Scan();
		};
	}
}