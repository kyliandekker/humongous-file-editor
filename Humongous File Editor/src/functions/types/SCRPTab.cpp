#include "functions/types/SCRPTab.h"

#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/UAUDIO_WAVE_READER_RESULT.h>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "lowlevel/OPCodes.h"
#include "functions/ChunkFunctions.h"

namespace HumongousFileEditor
{
	struct instruction
	{
		std::string name;
		size_t offset;
		size_t size;
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
			size_t offset = pos;
			uint8_t b = scrp_chunk.data[pos];

			chunk_reader::bytecode bytecode = chunk_reader::OPCODES_HE90[b];
			std::string bc = bytecode.name;
			pos++;

			size_t bytecode_size = bytecode.func(utils::add(fc->data, offset + sizeof(chunk_reader::HumongousHeader) + pos), scrp_size - pos);
			pos += bytecode_size;
			instructions.push_back({ bc , offset, bytecode_size });
		}

		return false;
	}
}