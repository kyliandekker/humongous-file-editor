#include "project/Resource.h"

#include <filesystem>
#include <iostream>
#include <wtypes.h>

namespace fs = std::filesystem;

namespace resource_editor
{
	namespace project
	{
		std::string ReplaceSlashes(std::string path)
		{
			std::replace(path.begin(), path.end(), '\\', '/');
			return path;
		}

		std::string GetName(std::string path)
		{
			return path.substr(path.find_last_of("/\\") + 1);
		}

		inline std::string getExtensionFromPath(std::string path, bool include_dot = false)
		{
			return path.substr(path.find_last_of(".") + (include_dot ? 0 : 1));
		}

        void Resource::Scan()
		{
			m_Path = ReplaceSlashes(m_Path);
			m_Name = GetName(m_Path);

			if (m_Name.empty())
			{
				m_Name = m_Path;
			}

			if (fs::is_directory(m_Path))
			{
				m_ResourceType = ResourceType::Folder;
				fs::directory_iterator ds = fs::directory_iterator(m_Path, std::filesystem::directory_options::skip_permission_denied);
				for (const auto& dirEntry : ds)
				{
					m_HasChildren = true;
				}
			}

			if (!m_FoldedOut)
			{
				return;
			}

			if (!fs::exists(m_Path))
			{
				return;
			}

			if (m_ResourceType == ResourceType::Folder)
			{
				m_Resources.clear();

				fs::directory_iterator ds = fs::directory_iterator(m_Path, std::filesystem::directory_options::skip_permission_denied);
				for (const auto& dirEntry : ds)
				{
					std::string pa = dirEntry.path().string().c_str();
					std::wstring s = std::wstring(pa.begin(), pa.end());

					// Check for hidden or system files.
					DWORD attributes = GetFileAttributes(s.c_str());
					if (attributes & FILE_ATTRIBUTE_HIDDEN || attributes & FILE_ATTRIBUTE_SYSTEM)
					{
						continue;
					}

					Resource resource;
					resource.m_Path = pa;
					resource.m_Parent = this;

					if (!fs::is_directory(pa))
					{
						std::string extension = getExtensionFromPath(pa);
						// Check if it is a MD file.
						if (extension.compare("HE0") == 0)
						{
							resource.m_ResourceType = ResourceType::HE0;
						}
						else if (extension.compare("(A)") == 0)
						{
							resource.m_ResourceType = ResourceType::A;
							resource.m_Show = false;
						}
						else if (extension.compare("HE2") == 0)
						{
							resource.m_ResourceType = ResourceType::HE2;
						}
						else if (extension.compare("HE4") == 0)
						{
							resource.m_ResourceType = ResourceType::HE4;
						}
						else
						{
							continue;
						}
					}

					m_Resources.push_back(resource);
				}
			}

			for (auto& resource : m_Resources)
			{
				resource.Scan();
			}

			m_HasChildren = m_Resources.size() != 0;
		}
	}
}