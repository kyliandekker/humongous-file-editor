#include "project/Project.h"

#include "utils/string.h"
#include "game/compilers/ResourceFileCompiler.h"
#include "game/compilers/TalkFileCompiler.h"
#include "game/compilers/SongFileCompiler.h"

resource_editor::project::Project resource_editor::project::project;

namespace resource_editor
{
	namespace project
	{
        void Project::LoadResource(project::Resource& a_Resource)
        {
			project::project.m_LoadedResources[(int)a_Resource.m_ResourceType] = &a_Resource;

			switch (a_Resource.m_ResourceType)
			{
				case project::ResourceType::HE0:
				case project::ResourceType::A:
				{
					project::Resource* a = nullptr;
					for (size_t i = 0; i < a_Resource.m_Parent->m_Resources.size(); i++)
					{
						project::Resource& resource = a_Resource.m_Parent->m_Resources[i];
						if (string_extensions::getFileWithoutExtension(resource.m_Name).compare(string_extensions::getFileWithoutExtension(a_Resource.m_Name)) == 0 && resource.m_ResourceType == project::ResourceType::A)
						{
							a = &resource;
							break;
						}
					}

					if (!a)
					{
						return;
					}

					project::project.m_LoadedResources[(int)project::ResourceType::A] = a;
					game::ResourceFileCompiler compiler;
					compiler.Decompile(a_Resource, a_Resource.m_GameResources);
					break;
				}
				case project::ResourceType::HE2:
				{
					game::TalkFileCompiler compiler;
					compiler.Decompile(a_Resource, a_Resource.m_GameResources);
					break;
				}
				case project::ResourceType::HE4:
				{
					game::SongFileCompiler compiler;
					compiler.Decompile(a_Resource, a_Resource.m_GameResources);
					break;
				}
			}
        }

		void Project::LoadResourceWithPath(project::Resource& a_Resource)
		{
			if (a_Resource.m_FileContainer.Open(a_Resource.m_Path))
			{
				LoadResource(a_Resource);
			}
		}

		void Project::ClearResources(project::Resource& a_Resource)
		{
			a_Resource.m_GameResources.clear();
		}

		void Project::UnloadResource(project::ResourceType a_ResourceType)
		{
			project::Resource* resource = project::project.m_LoadedResources[(int)a_ResourceType];
			if (!resource)
			{
				return;
			}

			resource->m_FileContainer.Unload();
			ClearResources(*resource);
			if (resource->m_ResourceType == project::ResourceType::HE0)
			{
				if (project::project.m_LoadedResources[(int)project::ResourceType::A])
				{
					project::Resource* resource = project::project.m_LoadedResources[(int)project::ResourceType::A];
					resource->m_FileContainer.Unload();
					ClearResources(*resource);
					project::project.m_LoadedResources[(int)project::ResourceType::A] = nullptr;
				}
			}
			project::project.m_LoadedResources[(int)a_ResourceType] = nullptr;
		}
    }
}