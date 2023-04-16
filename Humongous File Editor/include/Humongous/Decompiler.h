#pragma once

#include <string>

namespace uaudio
{
	namespace wave_reader
	{
		struct ChunkHeader;
	}
}

namespace HumongousFileEditor
{
	class SGEN_Chunk;
	class DIGI_Chunk;
	class SGHD_Chunk;
	class HSHD_Chunk;
	class SDAT_Chunk;
	class TALK_Chunk;

	class CFILE;
	class SongEntry;

	enum decompiler_err
	{
		err_ok,
		err_file_empty,
		err_chunk_size_exceeds_file_size,
		err_num_songs_exceeds_file_size,
		err_chunk_unrecognized,
		err_bad_file_start,
		err_chunk_name_missmatch
	};

	namespace decompiler
	{
		int getChunk(uaudio::wave_reader::ChunkHeader& chunk, CFILE& file);
		int getSGENChunk(SGEN_Chunk& chunk, CFILE& file);
		int getDIGIChunk(DIGI_Chunk& chunk, CFILE& file);
		int getSGHDChunk(SGHD_Chunk& chunk, CFILE& file);
		int getHSHDChunk(HSHD_Chunk& chunk, CFILE& file);
		int getSDATChunk(SDAT_Chunk& chunk, CFILE& file);
		int getTALKChunk(TALK_Chunk& chunk, CFILE& file);
		int ReadSongEntry(SongEntry* songEntry, CFILE& file, size_t& size, bool sgen = false);
		int throw_error(int error_code, CFILE& file);
		int decompile();
	}
}