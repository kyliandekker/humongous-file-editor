#pragma once

#include <string>
#include <map>
#include <vector>

namespace resource_editor
{
	namespace xml
	{
		class XMLItem
		{
		public:
			XMLItem() = default;

			std::string GetIndentation() const;
			std::string SerializeOpen() const;
			std::string SerializeClose() const;
			void AddProperty(const std::string& a_Name, const std::string& a_Value);
			std::string m_Name;
			std::vector<XMLItem> m_Childs;
			int m_Indent = 0;
		private:
			std::map<std::string, std::string> m_Properties;
		};
	}
}