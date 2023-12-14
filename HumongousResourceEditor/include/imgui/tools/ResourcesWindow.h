#pragma once

#include <vector>

#include "imgui/tools/BaseTool.h"
#include "game/GameResource.h"

namespace resource_editor
{
	namespace project
	{
		class Resource;
	}
	namespace imgui
	{
		class ResourcesWindow : public BaseTool
		{
		public:
			ResourcesWindow(std::string a_Name, project::Resource& a_Resource);
			void Render() override;
		private:
			project::Resource& m_Resource;
		};
	}
}