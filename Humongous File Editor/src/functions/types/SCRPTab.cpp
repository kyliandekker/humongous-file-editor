#include "functions/types/SCRPTab.h"

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "lowlevel/OPCodes.h"
#include "lowlevel/bytecode_data.h"

namespace HumongousFileEditor
{
	struct instruction
	{
		uint8_t code;
		std::string name;
		size_t offset;
		size_t size;
		chunk_reader::bytecode_data data;
	};
	bool SCRPTab::GetData(chunk_reader::FileContainer*& fc, size_t offset)
	{
		chunk_reader::SCRP_Chunk scrp_chunk;
		memcpy(&scrp_chunk, utils::add(fc->data, offset), sizeof(chunk_reader::SCRP_Chunk) - sizeof(scrp_chunk.data));
		scrp_chunk.data = utils::add(fc->data, offset + sizeof(chunk_reader::HumongousHeader));
		size_t scrp_size = scrp_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader);

		size_t pos = 0;
		std::vector<instruction> instructions;
		while (pos < scrp_size)
		{
			size_t bc_offset = pos;
			uint8_t b = scrp_chunk.data[pos];

			instruction instruction;
			instruction.code = b;

			chunk_reader::bytecode bytecode = chunk_reader::OPCODES_HE90[b];
			instruction.name = bytecode.name;
			pos++;

			chunk_reader::bytecode_data bcdata;

			size_t bytecode_size = bytecode.func(utils::add(fc->data, offset + sizeof(chunk_reader::HumongousHeader) + pos), scrp_size - pos, bcdata);
			pos += bytecode_size;

			instruction.offset = bc_offset;
			instruction.size = bytecode_size;
			instruction.data = bcdata;
			instructions.push_back(instruction);
		}

		return false;
	}
}