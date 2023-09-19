#pragma once

#include <string>

#include "functions/FileFunction.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileDecrypter : public FileFunction
		{
		public:
			bool Read();
		};
	}
}