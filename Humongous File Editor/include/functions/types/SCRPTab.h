#pragma once

#include "base/SoundTab.h"

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		class FileContainer;
	}

	class SCRPTab : public SoundTab
	{
	public:
		static bool GetData(chunk_reader::FileContainer*& fc, size_t offset);
	};
};