#pragma once

#include <array>

#include "project/Resource.h"

namespace resource_editor
{
	namespace project
	{
		class Project
		{
		public:
			Resource m_Folder;
			std::array<project::Resource*, 6> m_LoadedResources
			{
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
			};

			void LoadResource(project::Resource& a_Resource);
			void LoadResourceWithPath(project::Resource& a_Resource);
			void ClearResources(project::Resource& a_Resource);
			void UnloadResource(project::ResourceType a_ResourceType);
		};

		extern Project project;
	}
}