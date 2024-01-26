#include "game/decompiled_resource/ScriptResource.h"

#include <string>

#include "low_level/HumongousChunks.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "low_level/HumongousChunkDefinitions.h"
#include "cmd/OPCodes.h"
#include "system/Logger.h"
#include <allocators/DataStream.h>
#include <allocators/Data.h>

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
		{ }

		class Generic_Script_Chunk : public chunk_reader::HumongousHeader
		{
		public:
			Generic_Script_Chunk() = default;
			unsigned char* data = nullptr;
		};

		bool ScriptResource::GetScriptData(size_t a_Offset, void* a_Data, std::vector<ScriptInstruction>& a_Instructions, bool a_Validate)
		{
			a_Instructions.clear();

			size_t scrp_size = 0;
			size_t pos = 0;

			Generic_Script_Chunk chunk;
			memcpy(&chunk, a_Data, sizeof(chunk_reader::HumongousHeader));
			chunk.data = reinterpret_cast<unsigned char*>(low_level::utils::add(a_Data, sizeof(chunk_reader::HumongousHeader)));
			
			scrp_size = chunk.DataSize();

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
			// Verb
			else if (low_level::utils::chunkcmp(chunk.chunk_id, chunk_reader::VERB_CHUNK_ID) == 0)
			{
				unsigned char serial = unsigned char();
				// We skip a few bytes until we hit a null-terminator.
				while (pos < scrp_size)
				{
					unsigned char key = chunk.data[pos];
					pos++;
					serial += key;
					if (key == '\0')
					{
						break;
					}
					int16_t add_serial = *reinterpret_cast<int16_t*>(chunk.data + pos);
					serial += add_serial;
					pos += 2;
				}
			}

			if (low_level::utils::chunkcmp(chunk.chunk_id, chunk_reader::SCRP_CHUNK_ID) != 0 &&
				low_level::utils::chunkcmp(chunk.chunk_id, chunk_reader::LSCR_CHUNK_ID) != 0 &&
				low_level::utils::chunkcmp(chunk.chunk_id, chunk_reader::LSC2_CHUNK_ID) != 0 &&
				low_level::utils::chunkcmp(chunk.chunk_id, chunk_reader::EXCD_CHUNK_ID) != 0 &&
				low_level::utils::chunkcmp(chunk.chunk_id, chunk_reader::ENCD_CHUNK_ID) != 0 &&
				low_level::utils::chunkcmp(chunk.chunk_id, chunk_reader::VERB_CHUNK_ID) != 0)
			{
				assert(false);
				return false;
			}

			while (pos < scrp_size)
			{
				const uint8_t b = chunk.data[pos];

				if (chunk_reader::OPCODES_HE90.find(b) == chunk_reader::OPCODES_HE90.end())
				{
					LOGF(logger::LOGSEVERITY_ASSERT, "Found invalid bytecode. Expect code in OPCodes dictionary but got %c.", b);
				}

				ScriptInstruction instruction;
				instruction.m_Code = b;
				instruction.m_OffsetInSCRPChunk = sizeof(chunk_reader::HumongousHeader) + pos;
				instruction.m_SCRPOffset = a_Offset;
				instruction.m_AbsOffset = a_Offset + instruction.m_OffsetInSCRPChunk;

				// Move to the args (if any).
				pos++;

				ArgsAllocator args;
				chunk_reader::bytecode bytecode = chunk_reader::OPCODES_HE90[b];
				bytecode.m_Func(reinterpret_cast<unsigned char*>(low_level::utils::add(chunk.data, pos)), args);

				instruction.m_Name = bytecode.m_Name;
				instruction.m_Args = args;

				a_Instructions.push_back(instruction);

				if (a_Validate)
				{
					// Checking jumps and seeing if they are valid.
					if (chunk_reader::isJumpCode(instruction.m_Code, instruction.m_Args.m_Args.size()))
					{
						size_t offset = chunk_reader::jump(instruction, reinterpret_cast<unsigned char*>(a_Data), chunk.ChunkSize());

						const uint8_t byte = *reinterpret_cast<uint8_t*>(low_level::utils::add(a_Data, offset));

						offset += instruction.m_SCRPOffset;

						if (chunk_reader::OPCODES_HE90.find(byte) == chunk_reader::OPCODES_HE90.end())
						{
							LOGF(logger::LOGSEVERITY_ASSERT, "Jump command jumps to invalid bytecode. Expect code in OPCodes dictionary but got %c.", b);
						}

						if (offset > a_Offset + chunk.ChunkSize() || offset < a_Offset)
						{
							LOG(logger::LOGSEVERITY_ASSERT, "Script references outside script, should not be possible.");
						}
					}
				}

				pos += args.Size();
			}

			return true;
		}

		bool ScriptResource::GetScriptData(game::GameResource& a_Resource, std::vector<ScriptInstruction>& a_Instructions, bool a_Validate)
		{
			return GetScriptData(a_Resource.m_Offset, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.data(), a_Resource.m_Offset), a_Instructions, a_Validate);
		}

		bool ScriptResource::GetData(game::GameResource& a_Resource)
		{
			return GetScriptData(a_Resource, m_Instructions);
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