#pragma once

#include <cstdint>
#include <string>

#include "ArgsAllocator.h"

namespace resource_editor
{
	struct ScriptInstruction
	{
		uint8_t m_Code;
		std::string m_Name;
		size_t m_OffsetInSCRPChunk;
		ArgsAllocator m_Args;
		size_t m_SCRPOffset;
	};
}