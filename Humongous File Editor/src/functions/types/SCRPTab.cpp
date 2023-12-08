#include "functions/types/SCRPTab.h"

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "cmd/OPCodes.h"
#include "cmd/ArgsAllocator.h"
#include "systems/Logger.h"

namespace HumongousFileEditor
{
	struct Generic_Script_Chunk : chunk_reader::HumongousHeader
	{
		Generic_Script_Chunk() = default;
		unsigned char* data = nullptr;
	};

	bool SCRPTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, std::vector<instruction>& instructions)
	{
		size_t scrp_size = 0;
		size_t pos = 0;

		Generic_Script_Chunk chunk;
		memcpy(&chunk, utils::add(fc->data, offset), sizeof(chunk_reader::HumongousHeader));
		chunk.data = utils::add(fc->data, offset + sizeof(chunk_reader::HumongousHeader));
		scrp_size = chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader);

		if (utils::chunkcmp(chunk.chunk_id, chunk_reader::LSCR_CHUNK_ID) == 0)
		{
			// For some reason we skip the first byte.
			pos += 1;
		}
		// Local v8.
		else if (utils::chunkcmp(chunk.chunk_id, chunk_reader::LSC2_CHUNK_ID) == 0)
		{
			// For some reason we skip the first 4 bytes.
			pos += 4;
		}

		while (pos < scrp_size)
		{
			uint8_t b = chunk.data[pos];

			instruction instruction;
			instruction.code = b;
			instruction.offset_in_scrp_chunk = sizeof(chunk_reader::HumongousHeader) + pos;
			instruction.scrp_offset = offset;
			pos++;

			ArgsAllocator args;
			chunk_reader::bytecode bytecode = chunk_reader::OPCODES_HE90[b];
			bytecode.func(utils::add(fc->data, offset + sizeof(chunk_reader::HumongousHeader) + pos), args);

			instruction.name = bytecode.name;
			instruction.args = args;

			instructions.push_back(instruction);

			// Checking jumps and seeing if they are valid.
			if ((instruction.code == 0x5C // jump if
				|| instruction.code == 0x5D // jump if not
				|| instruction.code == 0x73) // jump
				|| (instruction.code == 0xA9 && instruction.args.args.size() == 2)
				)
			{
				size_t offset = HumongousFileEditor::chunk_reader::jump(instruction, fc->data);

				uint8_t tb = *reinterpret_cast<uint8_t*>(utils::add(fc->data, offset));

				if (chunk_reader::OPCODES_HE90.find(tb) == chunk_reader::OPCODES_HE90.end())
				{
					LOGF(logger::LOGSEVERITY_ASSERT, "Jump command jumps to invalid bytecode. Expect code in OPCodes dictionary but got %c", tb);
				}
			}

			pos += args.Size();
		}

		return true;
	}
}