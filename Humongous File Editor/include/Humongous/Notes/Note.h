#pragma once

#include <string>
#include <cstdint>

namespace HumongousFileEditor
{
	namespace notes
	{
		class Note
		{
		public:
			Note(std::string text, uint32_t index)
			{
				this->text = text;
				this->index = index;
			}
			uint32_t index;
			std::string text;
		};
	}
}