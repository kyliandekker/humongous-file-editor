#include "functions/types/SCRPTab.h"

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "cmd/OPCodes.h"
#include "cmd/ArgsAllocator.h"
#include "lowlevel/bytecode_data.h"

namespace HumongousFileEditor
{
	struct Generic_Script_Chunk : chunk_reader::HumongousHeader
	{
		Generic_Script_Chunk() = default;

		unsigned char* data = nullptr;
	};

	bool SCRPTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, std::vector<talk_instruction>& instructions)
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
			size_t bc_offset = pos;
			uint8_t b = chunk.data[pos];

			talk_instruction instruction;
			instruction.code = b;
			instruction.offset_in_scrp_chunk = sizeof(chunk_reader::HumongousHeader) + pos;
			instruction.scrp_offset = offset;
			pos++;

			ArgsAllocator args;
			chunk_reader::bytecode bytecode = chunk_reader::OPCODES_HE90[b];
			bytecode.func(utils::add(fc->data, offset + sizeof(chunk_reader::HumongousHeader) + pos), scrp_size - pos, args);

			instruction.name = bytecode.name;
			instruction.args = args;

			instructions.push_back(instruction);

			// Checking jumps and seeing if they are valid.
			if (instruction.code == 0x5C || instruction.code == 0x5D || instruction.code == 0x73)
			{
				int16_t offset = *reinterpret_cast<int16_t*>(utils::add(fc->data, 1 + instruction.scrp_offset + instruction.offset_in_scrp_chunk + instruction.args[0].offset));
				int16_t relative_offset_in_scrp = instruction.offset_in_scrp_chunk + offset;

				uint8_t tb = chunk.data[relative_offset_in_scrp];
				chunk_reader::bytecode bytecode = chunk_reader::OPCODES_HE90[tb];
			}
			// Checking wait jumps.
			if (instruction.code == 0x5C || instruction.code == 0x5D || instruction.code == 0x73)
			{
				if (instruction.args.args.size() == 2)
				{
					int16_t offset = *reinterpret_cast<int16_t*>(utils::add(fc->data, 1 + instruction.scrp_offset + instruction.offset_in_scrp_chunk + instruction.args[1].offset));
					int16_t relative_offset_in_scrp = instruction.offset_in_scrp_chunk + offset;

					uint8_t tb = chunk.data[relative_offset_in_scrp];
					chunk_reader::bytecode bytecode = chunk_reader::OPCODES_HE90[tb];
				}
			}

			pos += args.Size();
		}

		return true;
	}
}