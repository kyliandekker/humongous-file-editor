#include "game/compilers/Indexer/XMLItem.h"
#include <format>

namespace resource_editor
{
	namespace xml
	{
		std::string XMLItem::GetIndentation() const
		{
			std::string indentation;
			for (size_t i = 0; i < m_Indent; i++)
			{
				indentation += "\t";
			}
			return indentation;
		}

		std::string XMLItem::SerializeOpen() const
		{
			std::string propertyString;
			for (auto& prop : m_Properties)
			{
				propertyString += " " + prop.first + "=\"" + prop.second + "\"";
			}

			return std::format("{}<{}{}{}>", GetIndentation(), m_Name, propertyString, m_Childs.empty() ? "/" : "");
		}

		std::string XMLItem::SerializeClose() const
		{
			return std::format("{}</{}>", GetIndentation(), m_Name);
		}

		void XMLItem::AddProperty(const std::string& a_Name, const std::string& a_Value)
		{
			m_Properties[a_Name] = a_Value;
		}
	}
}