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
#include "imgui/tools/ExplorerWindow.h"
#include "cmd/OPCodes.h"
#include "utils/TempAllocator.h"
#include "utils/STempAllocator.h"

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

		bool TalkResource::GetData(game::GameResource& a_Resource)
		{
			std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);
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
			m_SDAT_Chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[1].m_Offset + sizeof(chunk_reader::HumongousHeader));

			m_NumSamples = m_SDAT_Chunk.ChunkSize();
			m_Samples = audio::utils::ToSample(m_SDAT_Chunk.data, m_SDAT_Chunk.ChunkSize());

			a_Resource.m_Properties.emplace(std::make_pair("4. Sample Rate", std::string(std::to_string(m_HSHD_Chunk.sample_rate) + "Hz")));
			a_Resource.m_Properties.emplace(std::make_pair("5. Channels", std::to_string(1) + " (mono)"));
			a_Resource.m_Properties.emplace(std::make_pair("6. Bits Per Sample", std::to_string(8)));
			a_Resource.m_Properties.emplace(std::make_pair("7. Size", std::string(std::to_string(m_SDAT_Chunk.ChunkSize()) + " bytes")));

			return true;
		}


		/*
			* General strategy:
				* First we replace the TALK chunk with the new sound data (very easy).
				* Afterwards, we go through every script in the (a) file and look for TALK scripts.
				* Every TALK script consists of the letter T, followed by an offset and a size 
				  (both relating to the absolute offset of the TALK chunk and the size of this chunk in the .HE2 file).
				* Check if the offset in a TALK script is greater than the offset of the TALK chunk that was changed.
					* Calculate the new offset. If the amount of digits in the offset remains the same replace it immediately.
					* If the amount of digits is smaller or greater, add it to a list of instructions we will deal with later.
				* If the offset in a TALK script is the same as the TALK chunk that was changed, check the size.
					* If the amount of digits is smaller or greater, add it to a list of instructions we will deal with later. 
				* Now we will deal with the instructions that were not easy to replace.
					* First look for jump commands in the same SCRP chunk as the instruction originated from.
						* If the jump command is after the TALK instruction and jumps back before the TALK instruction, it will need to be changed.
						* Same happens for jump commands that are after the TALK instruction and that jump to a point after the TALK instruction.
					* Once all instructions are dealt with, we replace the SCRP chunk with the changed jumps (the actual TALK instruction has not changed yet).
					* After that is dealt with, we then replace the whole SCRP chunk with the new size and changed TALK instruction.
				* Afterwards, we check if all commands even still work.
				* Then we repack the index file.
				* Then we unload and reload all files.
		*/
		bool TalkResource::ReplaceTALK(game::GameResource& a_Resource)
		{
			//std::string path;
			//uaudio::wave_reader::ChunkCollection chunkCollection;
			//if (!OpenResource(path, chunkCollection))
			//{
			//	return false;
			//}

			//uaudio::wave_reader::FMT_Chunk fmt_chunk;
			//uaudio::wave_reader::DATA_Chunk data_chunk;

			//if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
			//{
			//	LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" does not have a data chunk.", path.c_str());
			//	free(chunkCollection.data());
			//	return false;
			//}

			//if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
			//{
			//	LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" does not have a fmt chunk.", path.c_str());
			//	free(chunkCollection.data());
			//	return false;
			//}

			//chunk_reader::ChunkInfo oldChunk = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(a_Resource.m_Offset);

			//// Get TALK chunk for the raw audio data.
			//chunk_reader::TALK_Chunk talk_chunk = chunk_reader::TALK_Chunk(chunk_reader::TALK_CHUNK_ID);

			//std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);

			//std::vector<chunk_reader::ChunkInfo> desired = {
			//	chunk_reader::ChunkInfo(chunk_reader::HSHD_CHUNK_ID),
			//	chunk_reader::ChunkInfo(chunk_reader::SDAT_CHUNK_ID),
			//	chunk_reader::ChunkInfo(chunk_reader::SBNG_CHUNK_ID),
			//};

			//// We do not necessarily need the SBNG chunk.
			//if (low_level::utils::seekChildren(children, desired) < desired.size() - 1)
			//{
			//	return false;
			//}

			//assert(desired[0].m_Offset > -1 && desired[1].m_Offset > -1);

			//chunk_reader::SBNG_Chunk sbng_chunk;
			//if (desired[2].m_Offset == -1)
			//{
			//	LOGF(logger::LOGSEVERITY_INFO, "Could not find SBNG chunk when trying to replace resource with file \"%s\". Continuing anyways.", path.c_str());
			//	sbng_chunk.SetChunkSize(0);
			//}
			//else
			//{
			//	memcpy(&sbng_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[2].m_Offset), sizeof(chunk_reader::HumongousHeader));
			//	sbng_chunk.data = reinterpret_cast<unsigned char*>(low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[2].m_Offset + sizeof(chunk_reader::HumongousHeader)));
			//}

			//// Construct HSHD chunk.
			//chunk_reader::HSHD_Chunk hshd_chunk;
			//memcpy(&hshd_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[0].m_Offset), sizeof(chunk_reader::HSHD_Chunk));

			//// Construct SDAT chunk.
			//chunk_reader::SDAT_Chunk sdat_chunk = chunk_reader::SDAT_Chunk(chunk_reader::SDAT_CHUNK_ID);
			//sdat_chunk.SetChunkSize(data_chunk.chunkSize + sizeof(chunk_reader::ChunkHeader));

			//talk_chunk.SetChunkSize(
			//	sizeof(chunk_reader::HumongousHeader) + // DIGI chunk itself.
			//	hshd_chunk.ChunkSize() + // HSHD chunk.
			//	sbng_chunk.ChunkSize() + // Optional SBNG chunk.
			//	sdat_chunk.ChunkSize() // SDAT chunk.
			//);

			//DataStreamContainer new_talk_data = DataStreamContainer(static_cast<size_t>(talk_chunk.ChunkSize()));
			//new_talk_data.Write(&talk_chunk, sizeof(chunk_reader::TALK_Chunk));
			//new_talk_data.Write(&hshd_chunk, hshd_chunk.ChunkSize());
			//new_talk_data.Save();
			//if (sbng_chunk.ChunkSize() > 0)
			//{
			//	new_talk_data.Write(&sbng_chunk, sizeof(chunk_reader::HumongousHeader));
			//	new_talk_data.Save();
			//	new_talk_data.Write(sbng_chunk.data, sbng_chunk.DataSize());
			//}
			//new_talk_data.Write(&sdat_chunk, sizeof(chunk_reader::HumongousHeader));
			//new_talk_data.Write(data_chunk.data, data_chunk.chunkSize);
			//new_talk_data.Save();

			//a_Resource.m_Parent->m_FileContainer.Replace(a_Resource.m_Offset, new_talk_data.Data(), talk_chunk.ChunkSize());

			//free(chunkCollection.data());

			return true;
		}

        bool TalkResource::Replace(game::GameResource& a_Resource)
        {
			if (!ReplaceTALK(a_Resource))
			{
				return false;
			}


			//std::string path;
			//uaudio::wave_reader::ChunkCollection chunkCollection;
			//if (!OpenResource(path, chunkCollection))
			//{
			//	return false;
			//}

			//uaudio::wave_reader::FMT_Chunk fmt_chunk;
			//uaudio::wave_reader::DATA_Chunk data_chunk;

			//if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
			//{
			//	LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" does not have a data chunk.", path.c_str());
			//	free(chunkCollection.data());
			//	return false;
			//}

			//if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
			//{
			//	LOGF(logger::LOGSEVERITY_ERROR, "File \"%s\" does not have a fmt chunk.", path.c_str());
			//	free(chunkCollection.data());
			//	return false;
			//}

			//chunk_reader::ChunkInfo oldChunk = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(a_Resource.m_Offset);

			//// Get TALK chunk for the raw audio data.
			//chunk_reader::TALK_Chunk talk_chunk;
			//memcpy(&talk_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, a_Resource.m_Offset), sizeof(chunk_reader::TALK_Chunk));

			//std::vector<chunk_reader::ChunkInfo> children = a_Resource.m_Parent->m_FileContainer.GetChildren(a_Resource.m_Offset);

			//std::vector<chunk_reader::ChunkInfo> desired = {
			//	chunk_reader::ChunkInfo(chunk_reader::HSHD_CHUNK_ID),
			//	chunk_reader::ChunkInfo(chunk_reader::SDAT_CHUNK_ID),
			//	chunk_reader::ChunkInfo(chunk_reader::SBNG_CHUNK_ID),
			//};

			//// We do not necessarily need the SBNG chunk.
			//if (low_level::utils::seekChildren(children, desired) < desired.size() - 1)
			//{
			//	return false;
			//}

			//chunk_reader::SBNG_Chunk sbng_chunk;
			//if (desired[2].m_Offset == -1)
			//{
			//	LOGF(logger::LOGSEVERITY_INFO, "Could not find SBNG chunk when trying to replace resource with file \"%s\". Continuing anyways.", path.c_str());
			//	sbng_chunk.SetChunkSize(0);
			//}
			//else
			//{
			//	memcpy(&sbng_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[2].m_Offset), sizeof(chunk_reader::HumongousHeader));
			//	sbng_chunk.data = low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[2].m_Offset + sizeof(chunk_reader::HumongousHeader)), sbng_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader);
			//}

			//// Construct HSHD chunk.
			//chunk_reader::HSHD_Chunk hshd_chunk;
			//memcpy(&hshd_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[0].m_Offset), sizeof(chunk_reader::HSHD_Chunk));

			//// Construct SDAT chunk.
			//chunk_reader::SDAT_Chunk sdat_chunk;
			//size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
			//memcpy(&sdat_chunk, low_level::utils::add(a_Resource.m_Parent->m_FileContainer.m_Data, desired[1].m_Offset), header_size);
			//sdat_chunk.SetChunkSize(data_chunk.chunkSize + static_cast<uint32_t>(header_size));

			//talk_chunk.SetChunkSize(
			//	sizeof(chunk_reader::HumongousHeader) + // DIGI chunk itself.
			//	hshd_chunk.ChunkSize() + // HSHD chunk.
			//	sbng_chunk.ChunkSize() + // Optional SBNG chunk.
			//	sdat_chunk.ChunkSize() // SDAT chunk.
			//);

			//unsigned char* new_talk_data = reinterpret_cast<unsigned char*>(malloc(talk_chunk.ChunkSize()));
			//size_t pos = 0;
			//memcpy(new_talk_data, &talk_chunk, header_size);
			//pos += sizeof(talk_chunk);
			//memcpy(low_level::utils::add(new_talk_data, pos), &hshd_chunk, hshd_chunk.ChunkSize());
			//pos += hshd_chunk.ChunkSize();
			//if (sbng_chunk.ChunkSize() > 0)
			//{
			//	memcpy(low_level::utils::add(new_talk_data, pos), &sbng_chunk, sizeof(chunk_reader::HumongousHeader));
			//	pos += sizeof(chunk_reader::HumongousHeader);
			//	memcpy(low_level::utils::add(new_talk_data, pos), sbng_chunk.data, sbng_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader));
			//	pos += sbng_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader);
			//}
			//memcpy(low_level::utils::add(new_talk_data, pos), &sdat_chunk, sizeof(chunk_reader::HumongousHeader));
			//pos += sizeof(chunk_reader::HumongousHeader);
			//memcpy(low_level::utils::add(new_talk_data, pos), data_chunk.data, data_chunk.chunkSize);
			//pos += data_chunk.chunkSize;

			//a_Resource.m_Parent->m_FileContainer.Replace(a_Resource.m_Offset, new_talk_data, talk_chunk.ChunkSize());

			//free(chunkCollection.data());
			//free(new_talk_data);

			//project::Resource* he0 = imgui::explorer.m_LoadedResources[(int)project::ResourceType::HE0];
			//project::Resource* a = imgui::explorer.m_LoadedResources[(int)project::ResourceType::A];

			//if (a && he0)
			//{
			//	int32_t talk_chunk_new_size = talk_chunk.ChunkSize();
			//	std::string talk_chunk_new_size_str = std::to_string(talk_chunk_new_size);
			//	int32_t talk_chunk_old_size = oldChunk.ChunkSize();
			//	int32_t talk_chunk_dif_size = talk_chunk_new_size - talk_chunk_old_size;
			//	int32_t talk_chunk_offset = a_Resource.m_Offset;

			//	std::vector<ScriptInstruction> longer_instructions;

			//	unsigned char* full_data = reinterpret_cast<unsigned char*>(malloc(a->m_FileContainer.m_Size));
			//	if (!full_data)
			//	{
			//		return false;
			//	}
			//	memcpy(full_data, a->m_FileContainer.m_Data, a->m_FileContainer.m_Size);

			//	// First replace instructions that do not change in size.
			//	chunk_reader::ChunkInfo header = a->m_FileContainer.GetChunkInfo(0);
			//	while (header.m_Offset < a->m_FileContainer.m_Size)
			//	{
			//		if (low_level::utils::chunkcmp(header.chunk_id, chunk_reader::SCRP_CHUNK_ID) == 0 ||
			//			low_level::utils::chunkcmp(header.chunk_id, chunk_reader::LSCR_CHUNK_ID) == 0 ||
			//			low_level::utils::chunkcmp(header.chunk_id, chunk_reader::LSC2_CHUNK_ID) == 0 ||
			//			low_level::utils::chunkcmp(header.chunk_id, chunk_reader::EXCD_CHUNK_ID) == 0 ||
			//			low_level::utils::chunkcmp(header.chunk_id, chunk_reader::ENCD_CHUNK_ID) == 0 ||
			//			low_level::utils::chunkcmp(header.chunk_id, chunk_reader::VERB_CHUNK_ID) == 0)
			//		{
			//			std::vector<ScriptInstruction> instructions;

			//			game::GameResource resource;
			//			resource.m_Offset = header.m_Offset;
			//			resource.m_Parent = a;

			//			ScriptResource script = ScriptResource(resource);
			//			if (script.GetData(resource))
			//			{
			//				for (auto& instruction : script.m_Instructions)
			//				{
			//					// o72_getScriptString and o72_talkActor
			//					if (instruction.m_Code == 0x04 || instruction.m_Code == 0xBA)
			//					{
			//						TalkString talkie_string = TalkString(reinterpret_cast<char*>(low_level::utils::add(a->m_FileContainer.m_Data, 1 + instruction.m_SCRPOffset + instruction.m_OffsetInSCRPChunk + instruction.m_Args[0].m_Offset)));

			//						if (!talkie_string.IsTalkString())
			//						{
			//							continue;
			//						}

			//						size_t scrp_talk_offset = talkie_string.GetTalkOffset();

			//						// Check if the position string is longer.
			//						if (scrp_talk_offset > talk_chunk_offset)
			//						{
			//							size_t scrp_talk_new_offset = scrp_talk_offset + talk_chunk_dif_size;

			//							chunk_reader::ChunkInfo talk_chunk_info = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(scrp_talk_new_offset);
			//							assert(low_level::utils::chunkcmp(talk_chunk_info.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0);

			//							std::string new_talk_offset_str = std::to_string(scrp_talk_new_offset);

			//							// If the size of the new offset string and the old one is not the same (more digits).
			//							if (std::to_string(scrp_talk_offset).size() != new_talk_offset_str.size())
			//							{
			//								longer_instructions.push_back(instruction);
			//							}
			//							// If it is:
			//							else
			//							{
			//								size_t offset_in_a = talkie_string.GetTalkOffsetPos() + instruction.m_OffsetInSCRPChunk + instruction.m_Args[0].m_Offset + 1 + instruction.m_SCRPOffset;

			//								assert(*low_level::utils::add(a->m_FileContainer.m_Data, offset_in_a - 1) == 'T');
			//								memcpy(low_level::utils::add(full_data, offset_in_a), new_talk_offset_str.c_str(), new_talk_offset_str.size());
			//							}
			//						}
			//						// Check if the size string is longer.
			//						else if (scrp_talk_offset == talk_chunk_offset)
			//						{
			//							size_t talk_size = talkie_string.GetTalkSize();

			//							std::string talk_size_str = std::to_string(talkie_string.GetTalkSize());

			//							if (talk_chunk_new_size_str.size() != talk_size_str.size())
			//							{
			//								longer_instructions.push_back(instruction);
			//							}
			//							else
			//							{
			//								size_t offset_in_a = talkie_string.GetTalkSizePos() + instruction.m_OffsetInSCRPChunk + instruction.m_Args[0].m_Offset + 1 + instruction.m_SCRPOffset;
			//								memcpy(low_level::utils::add(full_data, offset_in_a), talk_chunk_new_size_str.c_str(), talk_chunk_new_size_str.size());
			//							}
			//						}
			//					}
			//				}
			//			}
			//		}
			//		header = a->m_FileContainer.GetNextChunk(header.m_Offset);
			//	}

			//	// Replace a with the scripts that did not change in length;
			//	a->m_FileContainer.Replace(0, full_data, a->m_FileContainer.m_Size);
			//	free(full_data);

			//	// Replace longer ones (in reverse).
			//	for (int t = shorter_longer_instructions.size() - 1; t > -1; t--)
			//	{
			//		// Step 1: Do the pre-calculations.
			//		ScriptInstruction& instruction = shorter_longer_instructions[t];

			//		TalkString talkie_string = TalkString(reinterpret_cast<char*>(low_level::utils::add(a->m_FileContainer.m_Data, 1 + instruction.m_SCRPOffset + instruction.m_OffsetInSCRPChunk + instruction.m_Args[0].m_Offset)));

			//		if (!talkie_string.IsTalkString())
			//		{
			//			continue;
			//		}

			//		int32_t script_size_difference = 0;
			//		int32_t script_replace_pos = 0;
			//		int32_t script_replacement_val = 0;
			//		int32_t script_old_replacement_val = 0;
			//		int32_t script_shorten_point = 0;

			//		size_t scrp_talk_offset = talkie_string.GetTalkOffset();
			//		std::string scrp_talk_offset_str = std::to_string(scrp_talk_offset);

			//		// Check if the position string is longer.
			//		if (scrp_talk_offset > talk_chunk_offset)
			//		{
			//			size_t scrp_talk_new_offset = scrp_talk_offset + talk_chunk_dif_size;

			//			chunk_reader::ChunkInfo talk_chunk_info = a_Resource.m_Parent->m_FileContainer.GetChunkInfo(scrp_talk_new_offset);
			//			assert(low_level::utils::chunkcmp(talk_chunk_info.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0);

			//			std::string new_talk_offset_str = std::to_string(scrp_talk_new_offset);

			//			// If the size of the new offset string and the old one is not the same (more digits).
			//			if (scrp_talk_offset_str.size() != new_talk_offset_str.size())
			//			{
			//				script_size_difference = scrp_talk_offset_str.size() - new_talk_offset_str.size();
			//				script_replacement_val = scrp_talk_new_offset;
			//				script_old_replacement_val = scrp_talk_offset;
			//				script_replace_pos = talkie_string.GetTalkOffsetPos() + instruction.m_OffsetInSCRPChunk + 1;
			//				script_shorten_point = script_replace_pos + new_talk_offset_str.size();
			//			}
			//			// If it is:
			//			else
			//			{
			//				assert(false);
			//			}
			//		}
			//		// Check if the size string is longer.
			//		else if (scrp_talk_offset == talk_chunk_offset)
			//		{
			//			size_t talk_size = talkie_string.GetTalkSize();

			//			std::string talk_size_str = std::to_string(talk_size);

			//			// If the size of the new offset string and the old one is not the same (more digits).
			//			if (talk_chunk_new_size_str.size() != talk_size_str.size())
			//			{
			//				script_size_difference = talk_chunk_new_size_str.size() - talk_size_str.size();
			//				script_replacement_val = talk_chunk_new_size;
			//				script_old_replacement_val = talk_size;
			//				script_replace_pos = talkie_string.GetTalkSizePos() + instruction.m_OffsetInSCRPChunk;
			//				script_shorten_point = script_replace_pos + talk_chunk_new_size_str.size();
			//			}
			//			// If it is:
			//			else
			//			{
			//				assert(false);
			//			}
			//		}

			//		chunk_reader::ChunkInfo scrp_info = a->m_FileContainer.GetChunkInfo(instruction.m_SCRPOffset);

			//		// TODO: Figure out if scripts only jump inside the script chunk or also to other scripts.
			//		// Step 2: Replace offsets in jump commands inside this script only (look into other scripts later if this ends up crashing games).

			//		game::GameResource resource = a_Resource;
			//		resource.m_Offset = instruction.m_SCRPOffset;
			//		resource.m_Parent = a;
			//		std::vector<ScriptInstruction> instructions;
			//		if (!ScriptResource::GetScriptData(resource, instructions))
			//		{
			//			return false;
			//		}

			//		unsigned char* scrp_data = reinterpret_cast<unsigned char*>(malloc(scrp_info.ChunkSize()));
			//		if (!scrp_data)
			//		{
			//			continue;
			//		}

			//		memset(scrp_data, 0, scrp_info.ChunkSize());
			//		memcpy(scrp_data, low_level::utils::add(a->m_FileContainer.m_Data, instruction.m_SCRPOffset), scrp_info.ChunkSize());

			//		for (auto& short_instruction : instructions)
			//		{
			//			int32_t offset_in_scrp = -1;

			//			// Normal wait instructions.
			//			if (short_instruction.m_Code == 0x5C // jump if
			//				|| short_instruction.m_Code == 0x5D // jump if not
			//				|| short_instruction.m_Code == 0x73 // jump
			//				)
			//			{
			//				offset_in_scrp =
			//					short_instruction.m_OffsetInSCRPChunk + // offset of instruction in scrp chunk.
			//					short_instruction.m_Args[0].m_Offset + // offset of arg (this will be 0 for index 0).
			//					1; // basically, we need to start reading from the actual arg offset. Adding 1 because without this, we'd get the offset of the instruction.
			//			}
			//			// Wait and jump.
			//			else if (short_instruction.m_Code == 0xA9) // wait and then jump.
			//			{
			//				if (short_instruction.m_Args.m_Args.size() < 2)
			//				{
			//					continue;
			//				}

			//				offset_in_scrp =
			//					short_instruction.m_OffsetInSCRPChunk + // offset of instruction in scrp chunk.
			//					short_instruction.m_Args[1].m_Offset + // offset of arg (this will be 0 for index 0). We want the second arg, cause the first arg says if we even have a second arg.
			//					1; // basically, we need to start reading from the actual arg offset. Adding 1 because without this, we'd get the offset of the instruction.
			//			}
			//			else
			//			{
			//				continue;
			//			}

			//			size_t offset_after_jump = chunk_reader::jump(short_instruction, a->m_FileContainer.m_Data, false);
			//			if ((short_instruction.m_OffsetInSCRPChunk < script_shorten_point && offset_after_jump > script_shorten_point) ||
			//				(short_instruction.m_OffsetInSCRPChunk > script_shorten_point && offset_after_jump < script_shorten_point))
			//			{
			//				int16_t jump_offset = *reinterpret_cast<int16_t*>(low_level::utils::add(a->m_FileContainer.m_Data, offset_in_scrp));
			//				jump_offset += script_size_difference;
			//				memcpy(low_level::utils::add(scrp_data, offset_in_scrp), &jump_offset, sizeof(int16_t));
			//			}
			//			else
			//			{
			//				continue;
			//			}
			//		}

			//		a->m_FileContainer.Replace(instruction.m_SCRPOffset, scrp_data, scrp_info.ChunkSize());
			//		free(scrp_data);

			//		scrp_data = reinterpret_cast<unsigned char*>(malloc(scrp_info.ChunkSize() + script_size_difference));
			//		if (!scrp_data)
			//		{
			//			continue;
			//		}

			//		// Copy everything until the string replacement.
			//		memset(scrp_data, 0, scrp_info.ChunkSize() + script_size_difference);
			//		memcpy(scrp_data, low_level::utils::add(a->m_FileContainer.m_Data, instruction.m_SCRPOffset), script_replace_pos);

			//		chunk_reader::HumongousHeader old_chunk = chunk_reader::HumongousHeader(scrp_info);
			//		old_chunk.SetChunkSize(scrp_info.ChunkSize() + script_size_difference);

			//		// Copy the new size.
			//		memcpy(low_level::utils::add(scrp_data, CHUNK_ID_SIZE), old_chunk.chunkSize, sizeof(old_chunk.chunkSize));

			//		// Copy the string replacement value.
			//		memcpy(
			//			low_level::utils::add(scrp_data, script_replace_pos),
			//			talk_chunk_new_size_str.c_str(),
			//			talk_chunk_new_size_str.size()
			//		);

			//		memcpy(
			//			// Offset in new data (so not full file).
			//			low_level::utils::add(scrp_data, script_replace_pos + talk_chunk_new_size_str.size()),
			//			// Offset in full file 
			//			low_level::utils::add(a->m_FileContainer.m_Data, scrp_info.m_Offset + script_replace_pos + std::to_string(script_old_replacement_val).size()),
			//			// Size of everything to copy.
			//			scrp_info.ChunkSize() - (script_replace_pos + std::to_string(script_old_replacement_val).size())
			//		);

			//		a->m_FileContainer.Replace(instruction.m_SCRPOffset, scrp_data, scrp_info.ChunkSize());
			//		free(scrp_data);
			//	}

			//	// Last check.
			//	header = a->m_FileContainer.GetChunkInfo(0);
			//	while (header.m_Offset < a->m_FileContainer.m_Size)
			//	{
			//		if (low_level::utils::chunkcmp(header.chunk_id, chunk_reader::SCRP_CHUNK_ID) == 0 ||
			//			low_level::utils::chunkcmp(header.chunk_id, chunk_reader::LSCR_CHUNK_ID) == 0 ||
			//			low_level::utils::chunkcmp(header.chunk_id, chunk_reader::LSC2_CHUNK_ID) == 0 ||
			//			low_level::utils::chunkcmp(header.chunk_id, chunk_reader::EXCD_CHUNK_ID) == 0 ||
			//			low_level::utils::chunkcmp(header.chunk_id, chunk_reader::ENCD_CHUNK_ID) == 0 ||
			//			low_level::utils::chunkcmp(header.chunk_id, chunk_reader::VERB_CHUNK_ID) == 0)
			//		{
			//			std::vector<ScriptInstruction> instructions;

			//			game::GameResource resource;
			//			resource.m_Offset = header.m_Offset;
			//			resource.m_Parent = a;

			//			ScriptResource script = ScriptResource(resource);

			//			if (!script.GetData(resource))
			//			{
			//				assert(false);
			//			}
			//		}
			//		header = a->m_FileContainer.GetNextChunk(header.m_Offset);
			//	}

			//	imgui::explorer.ClearResources(*a);
			//	imgui::explorer.LoadResource(*a);
			//}

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