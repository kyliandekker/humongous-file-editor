#include "functions/types/TALKTab.h"

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "functions/ChunkFunctions.h"

namespace HumongousFileEditor
{
	bool TALKTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk)
	{
		// Get HSHD chunk for the sample rate.
		size_t hshd_offset = chunk_reader::ChunkFunctions::GetOffsetChunk(fc, offset, { chunk_reader::HSHD_CHUNK_ID });
		if (hshd_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		// Get SDAT chunk for the raw audio data.
		size_t sdat_offset = chunk_reader::ChunkFunctions::GetOffsetChunk(fc, offset, { chunk_reader::SDAT_CHUNK_ID });
		if (sdat_offset == -1)
			return false;

		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = utils::add(fc->data, sdat_offset + header_size);

		return true;
	}
}