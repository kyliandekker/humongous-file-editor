#include "game/decompiled_resource/ScriptResource.h"

#include <string>

#include "low_level/HumongousChunks.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "low_level/HumongousChunkDefinitions.h"
#include "cmd/OPCodes.h"
#include "system/Logger.h"

namespace resource_editor
{
	namespace game
	{
		ScriptResource::ScriptResource(game::GameResource& a_Resource) : DecompiledResource()
		{
			GetData(a_Resource);
		}

		ScriptResource::ScriptResource(const ScriptResource& rhs) : DecompiledResource(rhs)
		{
		}

		ScriptResource::~ScriptResource()
		{
		}

		struct Generic_Script_Chunk : chunk_reader::HumongousHeader
		{
			Generic_Script_Chunk() = default;
			unsigned char* data = nullptr;
		};

		bool ScriptResource::GetData(game::GameResource& a_Resource)
		{
			size_t scrp_size = 0;
			size_t pos = 0;

			Generic_Script_Chunk chunk;
			memcpy(&chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, a_Resource.m_Offset), sizeof(chunk_reader::HumongousHeader));
			chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, a_Resource.m_Offset + sizeof(chunk_reader::HumongousHeader));
			scrp_size = chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader);

			if (low_level::utils::chunkcmp(chunk.chunk_id, chunk_reader::LSCR_CHUNK_ID) == 0)
			{
				// For some reason we skip the first byte.
				pos += 1;
			}
			// Local v8.
			else if (low_level::utils::chunkcmp(chunk.chunk_id, chunk_reader::LSC2_CHUNK_ID) == 0)
			{
				// For some reason we skip the first 4 bytes.
				pos += 4;
			}

			while (pos < scrp_size)
			{
				uint8_t b = chunk.data[pos];

				ScriptInstruction instruction;
				instruction.m_Code = b;
				instruction.m_OffsetInSCRPChunk = sizeof(chunk_reader::HumongousHeader) + pos;
				instruction.m_SCRPOffset = a_Resource.m_Offset;
				pos++;

				ArgsAllocator args;
				chunk_reader::bytecode bytecode = chunk_reader::OPCODES_HE90[b];
				bytecode.m_Func(low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, a_Resource.m_Offset + sizeof(chunk_reader::HumongousHeader) + pos), args);

				instruction.m_Name = bytecode.m_Name;
				instruction.m_Args = args;

				m_Instructions.push_back(instruction);

				// Checking jumps and seeing if they are valid.
				if ((instruction.m_Code == 0x5C // jump if
					|| instruction.m_Code == 0x5D // jump if not
					|| instruction.m_Code == 0x73) // jump
					|| (instruction.m_Code == 0xA9 && instruction.m_Args.m_Args.size() == 2)
					)
				{
					size_t offset = chunk_reader::jump(instruction, a_Resource.m_Parent->m_FileContainer.m_Data);

					uint8_t tb = *reinterpret_cast<uint8_t*>(low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, offset));

					if (chunk_reader::OPCODES_HE90.find(tb) == chunk_reader::OPCODES_HE90.end())
					{
						LOGF(logger::LOGSEVERITY_ASSERT, "Jump command jumps to invalid bytecode. Expect code in OPCodes dictionary but got %c", tb);
					}
				}

				pos += args.Size();
			}

			return true;
		}

        bool ScriptResource::Replace(game::GameResource& a_Resource)
        {
            return false;
        }

		bool ScriptResource::Save(game::GameResource& a_Resource)
		{
			return false;
		}
	}
}