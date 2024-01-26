#include "game/decompiled_resource/SongResource.h"

#include <vector>
#include <uaudio_wave_reader/WaveChunks.h>

#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "system/audio/AudioUtils.h"
#include "system/Logger.h"
#include "allocators/DataStream.h"
#include "allocators/ChunkCollectionWrapper.h"

namespace resource_editor
{
	namespace game
	{
		SongResource::SongResource(game::GameResource& a_Resource) : SoundResource(a_Resource)
		{
			GetData(a_Resource);
		}

		SongResource::SongResource(const SongResource& rhs) : SoundResource(rhs)
		{
			m_HSHD_Chunk = rhs.m_HSHD_Chunk;
			m_SDAT_Chunk = rhs.m_SDAT_Chunk;
			m_NumSamples = rhs.m_NumSamples;
			m_Samples = reinterpret_cast<double*>(malloc(m_NumSamples * sizeof(double)));
			if (m_Samples)
			{
				memcpy(m_Samples, rhs.m_Samples, m_NumSamples * sizeof(double));
			}
		}

		bool SongResource::GetData(game::GameResource& a_Resource)
		{
			// Get SGEN chunk first (tells us the position of the SONG).
			chunk_reader::SGEN_Chunk sgen_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(sgen_chunk, a_Resource.m_Offset);

			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(sgen_chunk.song_pos);
			if (children.size() == 0)
			{
				return false;
			}

			std::vector<chunk_reader::ChunkInfo> desired = {
				chunk_reader::ChunkInfo(chunk_reader::HSHD_CHUNK_ID),
				chunk_reader::ChunkInfo(chunk_reader::SDAT_CHUNK_ID),
			};

			if (low_level::utils::seekChildren(children, desired) < desired.size())
			{
				return false;
			}

			a_Resource.m_Parent->m_FileContainer.GetChunk(m_HSHD_Chunk, desired[0].m_Offset);

			a_Resource.m_Parent->m_FileContainer.GetChunk(m_SDAT_Chunk, desired[1].m_Offset, sizeof(chunk_reader::HumongousHeader));
			m_SDAT_Chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.data(), desired[1].m_Offset + sizeof(chunk_reader::HumongousHeader));

			m_NumSamples = m_SDAT_Chunk.ChunkSize();
			m_Samples = audio::utils::ToSample(m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize());

			a_Resource.m_Properties.emplace(std::make_pair("4. Sample Rate", std::string(std::to_string(m_HSHD_Chunk.sample_rate) + "Hz")));
			a_Resource.m_Properties.emplace(std::make_pair("5. Channels", std::to_string(1) + " (mono)"));
			a_Resource.m_Properties.emplace(std::make_pair("6. Bits Per Sample", std::to_string(8)));
			a_Resource.m_Properties.emplace(std::make_pair("7. Size", std::string(std::to_string(m_SDAT_Chunk.ChunkSize()) + " bytes")));

			return true;
		}

		bool SongResource::Replace(game::GameResource& a_Resource)
		{
			std::string path;
			uaudio::wave_reader::ChunkCollectionWrapper chunkCollection;
			if (!OpenResource(path, chunkCollection))
			{
				return false;
			}

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			uaudio::wave_reader::DATA_Chunk data_chunk;

			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
			{
				LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" does not have a data chunk.", path.c_str());
				return false;
			}

			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
			{
				LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" does not have a fmt chunk.", path.c_str());
				return false;
			}

			// Get SGEN chunk first (tells us the position of the SONG).
			chunk_reader::SGEN_Chunk sgen_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(sgen_chunk, a_Resource.m_Offset);

			// Get DIGI chunk for the raw audio data.
			size_t digi_offset = sgen_chunk.song_pos;

			chunk_reader::DIGI_Chunk digi_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(digi_chunk, digi_offset);

			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(sgen_chunk.song_pos);
			if (children.size() == 0)
			{
				return false;
			}

			std::vector<chunk_reader::ChunkInfo> desired = { chunk_reader::ChunkInfo(chunk_reader::HSHD_CHUNK_ID), chunk_reader::ChunkInfo(chunk_reader::SDAT_CHUNK_ID) };
			if (low_level::utils::seekChildren(children, desired) < desired.size())
			{
				return false;
			}

			chunk_reader::HSHD_Chunk hshd_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(hshd_chunk, desired[0].m_Offset);

			chunk_reader::SDAT_Chunk sdat_chunk;
			a_Resource.m_Parent->m_FileContainer.GetChunk(sdat_chunk, desired[1].m_Offset, sizeof(chunk_reader::HumongousHeader));
			sdat_chunk.SetChunkSize(data_chunk.ChunkSize() + static_cast<uint32_t>(sizeof(chunk_reader::HumongousHeader)));

			digi_chunk.SetChunkSize(
				sizeof(chunk_reader::DIGI_Chunk) + // DIGI chunk itself.
				hshd_chunk.ChunkSize() + // HSHD chunk.
				sdat_chunk.ChunkSize() // SDAT chunk.
			);

			DataStream new_data = DataStream(digi_chunk.ChunkSize());
			new_data.Write(&digi_chunk, sizeof(chunk_reader::DIGI_Chunk));
			new_data.Write(&hshd_chunk, hshd_chunk.ChunkSize());
			new_data.Write(&sdat_chunk, sizeof(chunk_reader::HumongousHeader));
			new_data.Write(data_chunk.data, data_chunk.ChunkSize());

			a_Resource.m_Parent->m_FileContainer.Replace(digi_offset, new_data.data(), new_data.size());

			int32_t dif_size = digi_chunk.ChunkSize() - sgen_chunk.song_size;

			chunk_reader::ChunkInfo next_chunk = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(0);
			while (next_chunk.m_Offset < a_Resource.m_Parent->m_FileContainer.size())
			{
				if (low_level::utils::chunkcmp(next_chunk.chunk_id, chunk_reader::SGEN_CHUNK_ID) == 0)
				{
					if (next_chunk.m_Offset >= a_Resource.m_Offset)
					{
						chunk_reader::SGEN_Chunk new_sgen_chunk;
						a_Resource.m_Parent->m_FileContainer.GetChunk(new_sgen_chunk, next_chunk.m_Offset);

						// Replace the size of the changed chunk.
						if (next_chunk.m_Offset == a_Resource.m_Offset)
						{
							new_sgen_chunk.song_size = digi_chunk.ChunkSize();
						}
						// Add the difference of size to the chunks after the current chunk.
						else
						{
							new_sgen_chunk.song_pos += dif_size;
						}

						a_Resource.m_Parent->m_FileContainer.Replace(next_chunk.m_Offset, reinterpret_cast<unsigned char*>(&new_sgen_chunk), sizeof(chunk_reader::SGEN_Chunk));
					}
				}
				next_chunk = a_Resource.m_Parent->m_FileContainer.GetNextChunk(next_chunk.m_Offset);
			}

			return true;
		}

		bool SongResource::Save(game::GameResource& a_Resource)
		{
			std::string path;
			if (SaveResource(path))
			{
				const bool saved = SaveSound(path, m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize() - sizeof(resource_editor::chunk_reader::SDAT_Chunk), m_HSHD_Chunk.sample_rate);
				if (saved)
				{
					system(path.c_str());
				}
				return saved;
			}
			return false;
		}
	}
}