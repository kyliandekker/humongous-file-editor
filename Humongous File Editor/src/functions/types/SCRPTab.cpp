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

			pos += args.Size();

			//if (b == 0x04 || b == 0xBA)
			//{
			//	instruction.name = bytecode.name;

			//	if (data_str != nullptr)
			//	{
			//		if (data_str[0] == 0x7F && data_str[1] == 'T')
			//		{
			//			instruction.full_str = std::string(reinterpret_cast<char*>(data_str));

			//			instruction.scrp_offset = offset;
			//			instruction.offset_in_scrp_chunk = sizeof(chunk_reader::HumongousHeader) + pos;

			//			size_t byte_pos_pos = instruction.full_str.find_first_of("T") + 1;
			//			size_t comma_pos = instruction.full_str.find_first_of(",");
			//			std::string talk_offset = instruction.full_str.substr(byte_pos_pos, comma_pos - byte_pos_pos);

			//			instruction.talk_offset = std::stoi(talk_offset);
			//			instruction.talk_offset_pos = byte_pos_pos + instruction.offset_in_scrp_chunk;

			//			size_t bracket_pos = instruction.full_str.find_first_of("[");
			//			std::string talk_size = instruction.full_str.substr(comma_pos + 1, (bracket_pos - 1) - (comma_pos + 1));
			//			instruction.talk_size_pos = comma_pos + 1 + instruction.offset_in_scrp_chunk;

			//			instruction.talk_size = std::stoi(talk_size);

			//			instructions.push_back(instruction);
			//		}
			//	}
			//}
			//pos += bytecode_size;
		}

		return true;
	}
}