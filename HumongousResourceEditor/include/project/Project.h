#pragma once

#include "project/Resource.h"

namespace resource_editor
{
	namespace project
	{
		class Project
		{
		public:
			Resource m_Folder;
		};

		extern Project project;
	}
}