#pragma once

#include <string>

#include "Humongous/HumongousChunks.h"

namespace HumongousFileEditor
{
	class CFILE;

	namespace decompiler
	{
		/// <summary>
		/// Returns a chunk based on the current seek pos of a file.
		/// </summary>
		/// <param name="file"></param>
		/// <returns></returns>
		void getChunk(uaudio::wave_reader::ChunkHeader& chunk, CFILE& file);
		/// <summary>
		/// Returns a sgen chunk based on the current seek pos of a file.
		/// </summary>
		/// <param name="file"></param>
		/// <returns></returns>
		SGEN_Chunk getSGENChunk(CFILE& file);
		/// <summary>
		/// Returns a digi chunk based on the current seek pos of a file.
		/// </summary>
		/// <param name="file"></param>
		/// <returns></returns>
		DIGI_Chunk getDIGIChunk(CFILE& file);
		/// <summary>
		/// Returns a sghd chunk based on the current seek pos of a file.
		/// </summary>
		/// <param name="file"></param>
		/// <returns></returns>
		SGHD_Chunk getSGHDChunk(CFILE& file);
		/// <summary>
		/// Returns a hshd chunk based on the current seek pos of a file.
		/// </summary>
		/// <param name="file"></param>
		/// <returns></returns>
		HSHD_Chunk getHSHDChunk(CFILE& file);
		/// <summary>
		/// Returns a sdat chunk based on the current seek pos of a file.
		/// </summary>
		/// <param name="file"></param>
		/// <returns></returns>
		SDAT_Chunk getSDATChunk(CFILE& file);
		/// <summary>
		/// Returns a talk chunk based on the current seek pos of a file.
		/// </summary>
		/// <param name="file"></param>
		/// <returns></returns>
		TALK_Chunk getTALKChunk(CFILE& file);
		/// <summary>
		/// Decompiles a file.
		/// </summary>
		/// <returns>0 on success, 1 on failure.</returns>
		int decompile();
	}
}