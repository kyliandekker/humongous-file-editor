#include "imgui/tools/ResourcesWindow.h"

#include "game/compilers/SongFileCompiler.h"
#include "game/compilers/ResourceFileCompiler.h"
#include "game/compilers/TalkFileCompiler.h"
#include "project/Resource.h"

namespace resource_editor
{
	namespace imgui
	{
		ResourcesWindow::ResourcesWindow(std::string a_Name, project::Resource& a_Resource) : BaseTool(0, a_Name, ""), m_Resource(a_Resource)
		{ }

		void ResourcesWindow::Render()
		{
			printf("Test");
		}
	}
}