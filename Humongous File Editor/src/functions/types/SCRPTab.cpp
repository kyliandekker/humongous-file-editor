#include "functions/types/SCRPTab.h"

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "lowlevel/OPCodes.h"
#include "lowlevel/bytecode_data.h"

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
			// For some reason we skip the first byte.
			pos += 4;
		}

		while (pos < scrp_size)
		{
			size_t bc_offset = pos;
			uint8_t b = chunk.data[pos];

			instruction instruction;
			instruction.code = b;

			chunk_reader::bytecode bytecode = chunk_reader::OPCODES_HE90[b];
			instruction.name = bytecode.name;
			pos++;

			size_t bytecode_size = bytecode.func(utils::add(fc->data, offset + sizeof(chunk_reader::HumongousHeader) + pos), scrp_size - pos, instruction.data_str);
			pos += bytecode_size;

			instruction.offset = bc_offset;
			instruction.size = bytecode_size + 1;
			instructions.push_back(instruction);
		}

		return true;
	}
}