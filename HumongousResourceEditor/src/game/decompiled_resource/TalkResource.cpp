#include "game/decompiled_resource/TalkResource.h"

#include <uaudio_wave_reader/ChunkCollection.h>
#include <uaudio_wave_reader/WaveChunks.h>
#include <vector>

#include "low_level/HumongousChunkDefinitions.h"
#include "low_level/ChunkInfo.h"
#include "low_level/Utils.h"
#include "game/GameResource.h"
#include "project/Resource.h"
#include "system/audio/AudioUtils.h"
#include "system/Logger.h"
#include "cmd/ScriptInstruction.h"
#include "imgui/tools/ExplorerWindow.h"
#include "game/decompiled_resource/ScriptResource.h"
#include "cmd/TalkInstruction.h"

namespace resource_editor
{
	namespace game
	{
		TalkResource::TalkResource(game::GameResource& a_Resource) : SoundResource(a_Resource)
		{
			GetData(a_Resource);
		}

		TalkResource::TalkResource(const TalkResource& rhs) : SoundResource(rhs)
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

		TalkResource::~TalkResource()
		{
			if (m_Samples)
			{
				free(m_Samples);
			}
			if (m_SDAT_Chunk.data)
			{
				free(m_SDAT_Chunk.data);
			}
		}

		bool TalkResource::GetData(game::GameResource& a_Resource)
		{
			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);
			std::vector<chunk_reader::ChunkInfo> desired = {
				chunk_reader::ChunkInfo(chunk_reader::HSHD_CHUNK_ID),
				chunk_reader::ChunkInfo(chunk_reader::SDAT_CHUNK_ID),
			};

			if (low_level::utils::seekChildren(children, desired) != desired.size())
				return false;

			memcpy(&m_HSHD_Chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[0].m_Offset), sizeof(chunk_reader::HSHD_Chunk));

			size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(m_SDAT_Chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
			memcpy(&m_SDAT_Chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[1].m_Offset), header_size);
			m_SDAT_Chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[1].m_Offset + header_size);

			m_NumSamples = m_SDAT_Chunk.ChunkSize();
			m_Samples = audio::utils::ToSample(m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize());

			return true;
		}

        bool TalkResource::Replace(game::GameResource& a_Resource)
        {
			std::string path;
			uaudio::wave_reader::ChunkCollection chunkCollection;
			if (!OpenResource(path, chunkCollection))
			{
				return false;
			}

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			uaudio::wave_reader::DATA_Chunk data_chunk;

			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
			{
				LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" does not have a data chunk.", path.c_str());
				free(chunkCollection.data());
				return false;
			}

			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
			{
				LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" does not have a fmt chunk.", path.c_str());
				free(chunkCollection.data());
				return false;
			}

			chunk_reader::ChunkInfo oldChunk = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(a_Resource.m_Offset);

			// Get TALK chunk for the raw audio data.
			chunk_reader::TALK_Chunk talk_chunk;
			memcpy(&talk_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, a_Resource.m_Offset), sizeof(chunk_reader::TALK_Chunk));

			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);

			std::vector<chunk_reader::ChunkInfo> desired = {
				chunk_reader::ChunkInfo(chunk_reader::HSHD_CHUNK_ID),
				chunk_reader::ChunkInfo(chunk_reader::SDAT_CHUNK_ID),
				chunk_reader::ChunkInfo(chunk_reader::SBNG_CHUNK_ID),
			};

			// We do not necessarily need the SBNG chunk.
			if (low_level::utils::seekChildren(children, desired) >= desired.size() -1)
				return false;

			chunk_reader::SBNG_Chunk sbng_chunk;
			if (desired[2].m_Offset == -1)
			{
				LOGF(logger::LOGSEVERITY_INFO, "Could not find SBNG chunk when trying to replace resource with file \"%s\". Continuing anyways.", path.c_str());
				sbng_chunk.SetChunkSize(0);
			}
			else
			{
				memcpy(&sbng_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[2].m_Offset), sizeof(chunk_reader::HumongousHeader));
				sbng_chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[2].m_Offset + sizeof(chunk_reader::HumongousHeader)), sbng_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader);
			}

			// Construct HSHD chunk.
			chunk_reader::HSHD_Chunk hshd_chunk;
			memcpy(&hshd_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[0].m_Offset), sizeof(chunk_reader::HSHD_Chunk));

			// Construct SDAT chunk.
			chunk_reader::SDAT_Chunk sdat_chunk;
			size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
			memcpy(&sdat_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[1].m_Offset), header_size);
			sdat_chunk.SetChunkSize(data_chunk.chunkSize + static_cast<uint32_t>(header_size));

			talk_chunk.SetChunkSize(
				sizeof(chunk_reader::HumongousHeader) + // DIGI chunk itself.
				hshd_chunk.ChunkSize() + // HSHD chunk.
				sbng_chunk.ChunkSize() + // Optional SBNG chunk.
				sdat_chunk.ChunkSize() // SDAT chunk.
			);

			unsigned char* new_talk_data = reinterpret_cast<unsigned char*>(malloc(talk_chunk.ChunkSize()));
			size_t pos = 0;
			memcpy(new_talk_data, &talk_chunk, header_size);
			pos += sizeof(talk_chunk);
			memcpy(low_level::utils::add(new_talk_data, pos), &hshd_chunk, hshd_chunk.ChunkSize());
			pos += hshd_chunk.ChunkSize();
			if (sbng_chunk.ChunkSize() > 0)
			{
				memcpy(low_level::utils::add(new_talk_data, pos), &sbng_chunk, sizeof(chunk_reader::HumongousHeader));
				pos += sizeof(chunk_reader::HumongousHeader);
				memcpy(low_level::utils::add(new_talk_data, pos), sbng_chunk.data, sbng_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader));
				pos += sbng_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader);
			}
			memcpy(low_level::utils::add(new_talk_data, pos), &sdat_chunk, sizeof(chunk_reader::HumongousHeader));
			pos += sizeof(chunk_reader::HumongousHeader);
			memcpy(low_level::utils::add(new_talk_data, pos), data_chunk.data, data_chunk.chunkSize);
			pos += data_chunk.chunkSize;

			a_Resource.m_Parent->m_FileContainer.Replace(a_Resource.m_Offset, new_talk_data, talk_chunk.ChunkSize());

			free(chunkCollection.data());
			free(new_talk_data);

			int32_t dif_size = static_cast<int32_t>(talk_chunk.ChunkSize()) - static_cast<int32_t>(oldChunk.ChunkSize());

			project::Resource* he0 = imgui::explorer.m_LoadedResources[(int)project::ResourceType::HE0];
			project::Resource* a = imgui::explorer.m_LoadedResources[(int)project::ResourceType::A];

			if (a && he0)
			{
				size_t new_size = talk_chunk.ChunkSize();

				std::vector<ScriptInstruction> shorter_longer_instructions{};

				unsigned char* full_data = reinterpret_cast<unsigned char*>(malloc(a->m_Parent->m_FileContainer.m_Size));
				memcpy(full_data, a->m_Parent->m_FileContainer.m_Data, a->m_Parent->m_FileContainer.m_Size);

				chunk_reader::ChunkInfo header = a->m_FileContainer.GetChunkInfo(0);
				while (header.m_Offset < a->m_FileContainer.m_Size)
				{
					if (low_level::utils::chunkcmp(header.chunk_id, chunk_reader::LSCR_CHUNK_ID) == 0 ||
						low_level::utils::chunkcmp(header.chunk_id, chunk_reader::LSC2_CHUNK_ID) == 0 ||
						low_level::utils::chunkcmp(header.chunk_id, chunk_reader::SCRP_CHUNK_ID) == 0 ||
						low_level::utils::chunkcmp(header.chunk_id, chunk_reader::EXCD_CHUNK_ID) == 0 ||
						low_level::utils::chunkcmp(header.chunk_id, chunk_reader::ENCD_CHUNK_ID) == 0
						)
					{
						std::vector<ScriptInstruction> instructions;

						game::GameResource resource;
						resource.m_Offset = header.m_Offset;
						resource.m_Parent = a;

						ScriptResource script = ScriptResource(resource);

						if (script.GetData(resource))
						{
							for (size_t k = 0; k < script.m_Instructions.size(); k++)
							{
								ScriptInstruction& instruction = script.m_Instructions[k];

								if (instruction.m_Code == 0x04 || instruction.m_Code == 0xBA)
								{
									TalkString talkie_string = TalkString(reinterpret_cast<char*>(low_level::utils::add(a->m_FileContainer.m_Data, 1 + instruction.m_SCRPOffset + instruction.m_OffsetInSCRPChunk + instruction.m_Args[0].m_Offset)));

									if (!talkie_string.IsTalkString())
									{
										continue;
									}

									size_t talk_offset = talkie_string.GetTalkOffset();

									// Check if the position string is longer.
									if (talk_offset > a_Resource.m_Offset)
									{
										size_t new_talk_offset = talk_offset + dif_size;

										std::string new_talk_offset_str = std::to_string(new_talk_offset);

										chunk_reader::ChunkInfo talk_chunk_info = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(new_talk_offset);
										assert(low_level::utils::chunkcmp(talk_chunk_info.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0);

										// If the new offset of this instruction is not the same as the previous one.
										if (std::to_string(talk_offset).size() != new_talk_offset_str.size())
										{
											shorter_longer_instructions.push_back(instruction);
										}
										else
										{
											size_t offset_in_a = talkie_string.GetTalkOffsetPos() + instruction.m_OffsetInSCRPChunk + instruction.m_Args[0].m_Offset + 1 + instruction.m_SCRPOffset;
											memcpy(low_level::utils::add(full_data, offset_in_a), new_talk_offset_str.c_str(), new_talk_offset_str.size());
										}
									}
									// Check if the size string is longer.
									else if (talk_offset == a_Resource.m_Offset)
									{
										size_t talk_size = talkie_string.GetTalkSize();

										std::string talk_size_str = std::to_string(talkie_string.GetTalkSize());

										if (std::to_string(talk_size).size() != talk_size_str.size())
										{
											shorter_longer_instructions.push_back(instruction);
										}
										else
										{
											size_t offset_in_a = talkie_string.GetTalkSizePos() + instruction.m_OffsetInSCRPChunk + instruction.m_Args[0].m_Offset + 1 + instruction.m_SCRPOffset;
											memcpy(low_level::utils::add(full_data, offset_in_a), talk_size_str.c_str(), talk_size_str.size());
										}
									}
								}
							}
						}
					}
					header = a->m_FileContainer.GetNextChunk(header.m_Offset);
				}

				// Replace a with the scripts that did not change in length;
				a->m_Parent->m_FileContainer.Replace(0, full_data, a->m_Parent->m_FileContainer.m_Size);
				free(full_data);

				// Replace longer ones (in reverse).
				for (int t = shorter_longer_instructions.size() - 1; t > -1; t--)
				{
					// First replace jumps.
					ScriptInstruction& sl_instruction = shorter_longer_instructions[t];

					TalkString talkie_string = TalkString(reinterpret_cast<char*>(low_level::utils::add(a->m_FileContainer.m_Data, 1 + sl_instruction.m_SCRPOffset + sl_instruction.m_OffsetInSCRPChunk + sl_instruction.m_Args[0].m_Offset)));

					if (!talkie_string.IsTalkString())
					{
						continue;
					}

					int32_t difference = 0;

					size_t talk_offset = talkie_string.GetTalkOffset();
					std::string talk_offset_str = std::to_string(talkie_string.GetTalkOffset());
					size_t talk_size = talkie_string.GetTalkSize();
					std::string talk_size_str = std::to_string(talk_size);

					size_t new_offset = talk_offset + dif_size;
					std::string new_offset_str = std::to_string(new_offset);

					// Check if the position string is longer
					if (talk_offset_str.size() != new_offset_str.size())
					{
						chunk_reader::ChunkInfo talk_chunk_info = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(new_offset);

						assert(low_level::utils::chunkcmp(talk_chunk_info.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0);
						difference = static_cast<int32_t>(new_offset_str.size()) - static_cast<int32_t>(talk_offset_str.size());
					}
					else if (talkie_string.GetTalkOffset() == a_Resource.m_Offset && std::to_string(talkie_string.GetTalkSize()).size() != talk_size_str.size())
					{
						difference = static_cast<int32_t>(talk_size_str.size()) - static_cast<int32_t>(talk_size_str.size());

						chunk_reader::ChunkInfo talk_chunk_info = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(new_offset);
						assert(low_level::utils::chunkcmp(talk_chunk_info.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0);
						assert(talk_chunk_info.ChunkSize() == talk_size);
					}

					chunk_reader::ChunkInfo scrp_info = a->m_Parent->m_FileContainer.GetChunkInfo(sl_instruction.m_SCRPOffset);

					game::GameResource resource = a_Resource;
					resource.m_Offset = sl_instruction.m_SCRPOffset;
					std::vector<ScriptInstruction> instructions;
					if (!ScriptResource::GetScriptData(resource, instructions))
					{
						return false;
					}

					unsigned char* scrp_data = reinterpret_cast<unsigned char*>(malloc(scrp_info.ChunkSize()));
					if (!scrp_data)
					{
						continue;
					}

					memset(scrp_data, 0, scrp_info.ChunkSize());
					memcpy(scrp_data, low_level::utils::add(a->m_Parent->m_FileContainer.m_Data, sl_instruction.m_SCRPOffset), scrp_info.ChunkSize());
				}
			}

			return true;
        }

		bool TalkResource::Save(game::GameResource& a_Resource)
		{
			std::string path;
			if (SaveResource(path))
			{
				bool saved = SaveSound(path, m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize() - sizeof(resource_editor::chunk_reader::SDAT_Chunk), m_HSHD_Chunk.sample_rate);
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