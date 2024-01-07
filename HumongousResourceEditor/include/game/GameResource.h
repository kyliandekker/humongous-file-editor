#pragma once

#include <string>
#include <vector>
#include <map>

#include "GameResourceType.h"

namespace resource_editor
{
	namespace project
	{
		class Resource;
	}

	namespace game
	{
		class GameResource
		{
		public:
			size_t m_Offset = 0;
			std::string m_Name = "";
			GameResourceType m_Type;
			std::vector<GameResource> m_Resources;
			bool m_OpenedWindow = false;
			project::Resource* m_Parent = nullptr;
			std::map<std::string, std::string> m_Properties;
		};
	}
}