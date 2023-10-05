#include "functions/types/TALKTab.h"

#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/UAUDIO_WAVE_READER_RESULT.h>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <wtypes.h>
#include <commdlg.h>
#include <map>
#include <cassert>
#include <vector>

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "functions/ChunkFunctions.h"
#include "functions/ResourceGatherer.h"
#include "functions/types/SCRPTab.h"
#include "cmd/talk_string.h"
#include "HumongousEditorForm.h"

namespace HumongousFileEditor
{
	struct dlfl_offset
	{
		size_t offset;
		size_t actual_offset;
	};
	struct pair
	{
		size_t rmim_offset;
		size_t offset;
		size_t actual_offset;
	};
	struct diri_pair3
	{
		size_t rmim_offset;

		size_t offset;
		size_t actual_offset_of_offset;

		size_t size;
		size_t actual_offset_of_size;
	};
	size_t TALKTab::ReplaceInTalk(std::string path, chunk_reader::FileContainer*& fc, size_t offset, uaudio::wave_reader::DATA_Chunk& data_chunk, uaudio::wave_reader::FMT_Chunk& fmt_chunk)
	{
		if (!utils::ends_with(path, ".wav"))
			path += ".wav";

		size_t wave_size = 0;
		if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::FTell(path.c_str(), wave_size)))
			return 0;

		void* chunk_alloc = malloc(wave_size);
		uaudio::wave_reader::ChunkCollection chunkCollection(chunk_alloc, wave_size);
		if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::LoadWave(path.c_str(), chunkCollection)))
		{
			free(chunk_alloc);
			return 0;
		}

		if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
		{
			free(chunk_alloc);
			return 0;
		}

		if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
		{
			free(chunk_alloc);
			return 0;
		}

		if (fmt_chunk.byteRate != 11025)
		{
			free(chunk_alloc);
			return 0;
		}

		if (fmt_chunk.sampleRate != 11025)
		{
			free(chunk_alloc);
			return 0;
		}

		if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8)
		{
			free(chunk_alloc);
			return 0;
		}

		if (fmt_chunk.numChannels != uaudio::wave_reader::WAVE_CHANNELS_MONO)
		{
			free(chunk_alloc);
			return 0;
		}

		// Get TALK chunk for the raw audio data.
		size_t talk_offset = offset;

		chunk_reader::TALK_Chunk talk_chunk;
		memcpy(&talk_chunk, utils::add(fc->data, talk_offset), sizeof(chunk_reader::TALK_Chunk));

		std::vector<chunk_reader::ChunkInfo> children = fc->GetChildren(talk_offset);
		size_t hshd_offset = -1;
		size_t sdat_offset = -1;
		size_t sbng_offset = -1;
		for (size_t j = 0; j < children.size(); j++)
		{
			if (utils::chunkcmp(children[j].chunk_id, chunk_reader::HSHD_CHUNK_ID) == 0)
				hshd_offset = children[j].offset;
			if (utils::chunkcmp(children[j].chunk_id, chunk_reader::SDAT_CHUNK_ID) == 0)
				sdat_offset = children[j].offset;
			if (utils::chunkcmp(children[j].chunk_id, chunk_reader::SBNG_CHUNK_ID) == 0)
				sbng_offset = children[j].offset;
		}

		if (hshd_offset == -1)
		{
			free(chunk_alloc);
			return 0;
		}

		if (hshd_offset == -1)
		{
			free(chunk_alloc);
			return 0;
		}

		chunk_reader::SBNG_Chunk sbng_chunk;
		if (sbng_offset == -1)
			sbng_chunk.SetChunkSize(0);
		else
		{
			memcpy(&sbng_chunk, utils::add(fc->data, sbng_offset), sizeof(chunk_reader::HumongousHeader));
			sbng_chunk.data = utils::add(fc->data, sbng_offset + sizeof(chunk_reader::HumongousHeader)), sbng_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader);
		}

		chunk_reader::HSHD_Chunk hshd_chunk;
		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		chunk_reader::SDAT_Chunk sdat_chunk;
		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.SetChunkSize(data_chunk.chunkSize + static_cast<uint32_t>(header_size));

		talk_chunk.SetChunkSize(
			sizeof(chunk_reader::HumongousHeader) + // DIGI chunk itself.
			hshd_chunk.ChunkSize() + // HSHD chunk.
			sbng_chunk.ChunkSize() + // Optional SBNG chunk.
			sdat_chunk.ChunkSize() // SDAT chunk.
		);

		unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(talk_chunk.ChunkSize()));
		size_t pos = 0;
		memcpy(new_data, &talk_chunk, header_size);
		pos += sizeof(talk_chunk);
		memcpy(utils::add(new_data, pos), &hshd_chunk, hshd_chunk.ChunkSize());
		pos += hshd_chunk.ChunkSize();
		if (sbng_chunk.ChunkSize() > 0)
		{
			memcpy(utils::add(new_data, pos), &sbng_chunk, sizeof(chunk_reader::HumongousHeader));
			pos += sizeof(chunk_reader::HumongousHeader);
			memcpy(utils::add(new_data, pos), sbng_chunk.data, sbng_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader));
			pos += sbng_chunk.ChunkSize() - sizeof(chunk_reader::HumongousHeader);
		}
		memcpy(utils::add(new_data, pos), &sdat_chunk, sizeof(chunk_reader::HumongousHeader));
		pos += sizeof(chunk_reader::HumongousHeader);
		memcpy(utils::add(new_data, pos), data_chunk.data, data_chunk.chunkSize);
		pos += data_chunk.chunkSize;

		fc->Replace(talk_offset, new_data, talk_chunk.ChunkSize());

		free(chunk_alloc);
		free(new_data);

		return talk_chunk.ChunkSize();
	}
	bool TALKTab::ReplaceResource(chunk_reader::FileContainer*& fc, size_t offset)
	{
		unsigned char* prev_data = reinterpret_cast<unsigned char*>(malloc(fc->size));
		memcpy(prev_data, fc->data, fc->size);

		std::string path;
		uaudio::wave_reader::DATA_Chunk data_chunk;
		uaudio::wave_reader::FMT_Chunk fmt_chunk;
		if (SoundTab::ReplaceResource(path, fmt_chunk, data_chunk))
		{
			chunk_reader::ChunkInfo oldChunk = fc->GetChunkInfo(offset);
			size_t talk_size = ReplaceInTalk(path, fc, offset, data_chunk, fmt_chunk);

			if (talk_size == 0)
				return false;

			int32_t dif_size = static_cast<int32_t>(talk_size) - static_cast<int32_t>(oldChunk.ChunkSize());

			unsigned char* full_data = reinterpret_cast<unsigned char*>(malloc(files::FILES.a->size));
			memcpy(full_data, files::FILES.a->data, files::FILES.a->size);

			std::string talk_size_str = std::to_string(talk_size);

			std::vector<talk_instruction> shorter_longer_instructions{};

			chunk_reader::ChunkInfo header = files::FILES.a->GetChunkInfo(0);
			while (header.offset < files::FILES.a->size)
			{
				if (utils::chunkcmp(header.chunk_id, chunk_reader::LSCR_CHUNK_ID) == 0 ||
					utils::chunkcmp(header.chunk_id, chunk_reader::LSC2_CHUNK_ID) == 0 ||
					utils::chunkcmp(header.chunk_id, chunk_reader::SCRP_CHUNK_ID) == 0 ||
					utils::chunkcmp(header.chunk_id, chunk_reader::EXCD_CHUNK_ID) == 0 ||
					utils::chunkcmp(header.chunk_id, chunk_reader::ENCD_CHUNK_ID) == 0
					)
				{
					std::vector<talk_instruction> instructions;
					if (SCRPTab::GetData(files::FILES.a, header.offset, instructions))
					{
						for (size_t k = 0; k < instructions.size(); k++)
						{
							talk_instruction& instruction = instructions[k];

							if (instruction.code == 0x04 || instruction.code == 0xBA)
							{
								talk_string talkie_string = talk_string(reinterpret_cast<char*>(utils::add(files::FILES.a->data, 1 + instruction.scrp_offset + instruction.offset_in_scrp_chunk + instruction.args[0].offset)));

								if (!talkie_string.IsTalkString())
									continue;

								size_t talk_offset = talkie_string.GetTalkOffset();

								if (talk_offset > offset)
								{
									size_t talk_offset_pos = talkie_string.GetTalkOffsetPos() + instruction.offset_in_scrp_chunk + instruction.args[0].offset + 1;

									size_t new_offset = talk_offset + dif_size;
									std::string offset_str = std::to_string(new_offset);

									chunk_reader::ChunkInfo talk_chunk_info = files::FILES.he2->GetChunkInfo(new_offset);
									assert(utils::chunkcmp(talk_chunk_info.chunk_id, chunk_reader::TALK_CHUNK_ID) == 0);

									// If the new offset of this instruction is not the same as the previous one.
									if (std::to_string(talk_offset).size() != offset_str.size())
										shorter_longer_instructions.push_back(instruction);
									else
										memcpy(utils::add(full_data, talk_offset_pos + instruction.scrp_offset), offset_str.c_str(), offset_str.size());
								}
								if (talk_offset == offset)
								{
									size_t talk_size = talkie_string.GetTalkSize();
									size_t talk_size_pos = talkie_string.GetTalkSizePos() + instruction.offset_in_scrp_chunk + instruction.args[0].offset + 1;

									if (std::to_string(talk_size).size() != talk_size_str.size())
										shorter_longer_instructions.push_back(instruction);
									else
										memcpy(utils::add(full_data, talk_size_pos + instruction.scrp_offset), talk_size_str.c_str(), talk_size_str.size());
								}
							}
						}
					}
				}
				chunk_reader::ChunkInfo next = files::FILES.a->GetNextChunk(header.offset);
				header = next;
			}

			files::FILES.a->Replace(0, full_data, files::FILES.a->size);
			free(full_data);

			if (!shorter_longer_instructions.empty())
			{
				System::Windows::Forms::MessageBox::Show("The voice file you are trying to replace causes other voice files to shift. Fixing this has been a headache for me, so for the time being if you see this message, make the new voiceline shorter..", "ERROR", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
				free(fc->data);
				fc->data = prev_data;
				return false;
			}

			free(prev_data);


			//// Replace longer ones.
			//for (size_t t = shorter_longer_instructions.size(); t-- > 0; )
			//{
			//	// First replace jumps.

			//	talk_instruction& sl_instruction = shorter_longer_instructions[t];

			//	talk_string talkie_string = talk_string(reinterpret_cast<char*>(utils::add(files::FILES.a->data, 1 + sl_instruction.scrp_offset + sl_instruction.offset_in_scrp_chunk + sl_instruction.args[0].offset)));

			//	if (!talkie_string.IsTalkString())
			//		continue;

			//	int32_t difference = 0;

			//	size_t new_offset = talkie_string.GetTalkOffset() + dif_size;
			//	if (std::to_string(talkie_string.GetTalkOffset()).size() != std::to_string(new_offset).size())
			//		difference = static_cast<int32_t>(std::to_string(new_offset).size()) - static_cast<int32_t>(std::to_string(talkie_string.GetTalkOffset()).size());
			//	else if (talkie_string.GetTalkOffset() == offset && std::to_string(talkie_string.GetTalkSize()).size() != talk_size_str.size())
			//		difference = static_cast<int32_t>(talk_size_str.size()) - static_cast<int32_t>(std::to_string(talkie_string.GetTalkSize()).size());

			//	chunk_reader::ChunkInfo scrp_info = files::FILES.a->GetChunkInfo(sl_instruction.scrp_offset);

			//	std::vector<talk_instruction> instructions;
			//	if (SCRPTab::GetData(files::FILES.a, sl_instruction.scrp_offset, instructions))
			//	{
			//		unsigned char* scrp_data = reinterpret_cast<unsigned char*>(malloc(scrp_info.ChunkSize()));
			//		if (!scrp_data)
			//			continue;

			//		ZeroMemory(scrp_data, scrp_info.ChunkSize());
			//		memcpy(scrp_data, utils::add(files::FILES.a->data, sl_instruction.scrp_offset), scrp_info.ChunkSize());

			//		for (size_t i = 0; i < instructions.size(); i++)
			//		{
			//			talk_instruction& instruction = instructions[i];

			//			// if, if not, jump to
			//			if (instruction.code == 0x5C || instruction.code == 0x5D || instruction.code == 0x73)
			//			{
			//				uint16_t offset = *reinterpret_cast<uint16_t*>(utils::add(files::FILES.a->data, 1 + instruction.scrp_offset + instruction.offset_in_scrp_chunk + instruction.args[0].offset));
			//				uint16_t relative_offset_in_scrp = instruction.offset_in_scrp_chunk + offset;

			//				if (offset > sl_instruction.offset_in_scrp_chunk || instruction.offset_in_scrp_chunk > sl_instruction.offset_in_scrp_chunk)
			//				{
			//					offset += difference;
			//					memcpy(utils::add(scrp_data, instruction.offset_in_scrp_chunk + instruction.args[0].offset + 1), &offset, sizeof(uint16_t));
			//				}
			//			}
			//			// wait.
			//			else if (instruction.code == 0xA9)
			//			{
			//				if (instruction.args.args.size() < 2)
			//					continue;

			//				size_t offset = instruction.offset_in_scrp_chunk + *reinterpret_cast<uint16_t*>(utils::add(files::FILES.a->data, 1 + instruction.scrp_offset + instruction.offset_in_scrp_chunk + instruction.args[1].offset));
			//				uint16_t relative_offset_in_scrp = instruction.offset_in_scrp_chunk + offset;

			//				if (offset > sl_instruction.offset_in_scrp_chunk || instruction.offset_in_scrp_chunk > sl_instruction.offset_in_scrp_chunk)
			//				{
			//					offset += difference;
			//					memcpy(utils::add(scrp_data, instruction.offset_in_scrp_chunk + instruction.args[1].offset + 1), &offset, sizeof(uint16_t));
			//				}
			//			}
			//		}

			//		files::FILES.a->Replace(sl_instruction.scrp_offset, scrp_data, scrp_info.ChunkSize());
			//		free(scrp_data);
			//	}

			//	// Collect all rnims.
			//	std::vector<size_t> rmim_offsets;
			//	rmim_offsets.push_back(0);

			//	chunk_reader::ChunkInfo rmim_header = files::FILES.a->GetChunkInfo(0);
			//	while (rmim_header.offset < files::FILES.a->size)
			//	{
			//		if (utils::chunkcmp(rmim_header.chunk_id, chunk_reader::RMIM_CHUNK_ID) == 0)
			//			rmim_offsets.push_back(rmim_header.offset);
			//		chunk_reader::ChunkInfo next = files::FILES.a->GetNextChunk(rmim_header.offset);
			//		rmim_header = next;
			//	}

			//	size_t replace_on_offset = 0;
			//	size_t old_replace_value = 0;
			//	size_t replace_value = 0;

			//	if (std::to_string(talkie_string.GetTalkOffset()).size() != std::to_string(new_offset).size())
			//	{
			//		replace_on_offset = talkie_string.GetTalkOffsetPos() + sl_instruction.offset_in_scrp_chunk + sl_instruction.args[0].offset + 1;
			//		old_replace_value = talkie_string.GetTalkOffset();
			//		replace_value = new_offset;
			//	}
			//	else if (talkie_string.GetTalkOffset() == offset && std::to_string(talkie_string.GetTalkSize()).size() != talk_size_str.size())
			//	{
			//		replace_on_offset = talkie_string.GetTalkSizePos() + sl_instruction.offset_in_scrp_chunk + sl_instruction.args[0].offset + 1;
			//		old_replace_value = talkie_string.GetTalkSize();
			//		replace_value = talk_size;
			//	}

			//	uint32_t new_size = static_cast<uint32_t>(scrp_info.ChunkSize()) + difference;
			//	unsigned char* scrp_data = reinterpret_cast<unsigned char*>(malloc(new_size));
			//	if (!scrp_data)
			//		continue;

			//	ZeroMemory(scrp_data, new_size);
			//	memcpy(scrp_data, utils::add(files::FILES.a->data, sl_instruction.scrp_offset), replace_on_offset);

			//	chunk_reader::HumongousHeader old_chunk = files::FILES.a->GetChunk(sl_instruction.scrp_offset);
			//	old_chunk.SetChunkSize(new_size);
			//	memcpy(
			//		scrp_data,
			//		&old_chunk,
			//		sizeof(old_chunk)
			//	);

			//	// Copy the replace value.
			//	std::string replace_value_str = std::to_string(replace_value);
			//	memcpy(
			//		utils::add(scrp_data, replace_on_offset),
			//		replace_value_str.c_str(),
			//		replace_value_str.size()
			//	);

			//	// Copy the rest of the data.
			//	size_t rest_data_place_pos = replace_on_offset + replace_value_str.size();
			//	size_t rest_data_pos = sl_instruction.scrp_offset + replace_on_offset + std::to_string(old_replace_value).size();
			//	size_t rest_data_size = scrp_info.ChunkSize() - rest_data_place_pos + difference;
			//	memcpy(
			//		utils::add(scrp_data, rest_data_place_pos),
			//		utils::add(files::FILES.a->data, rest_data_pos),
			//		rest_data_size
			//	);

			//	files::FILES.a->Replace(sl_instruction.scrp_offset, scrp_data, new_size);
			//	free(scrp_data);

			//	// You need to check in which lflf the scrp chunk is and only change things in that specific lflf.
			//	size_t belongs_to_rmim = -1;
			//	for (size_t i = 0; i < rmim_offsets.size(); i++)
			//	{
			//		size_t n = (i + 1) >= rmim_offsets.size() ? files::FILES.a->size : rmim_offsets[i + 1];
			//		if (sl_instruction.scrp_offset > rmim_offsets[i] && sl_instruction.scrp_offset < n)
			//			belongs_to_rmim = i;
			//	}

			//	chunk_reader::ChunkInfo he0_header = files::FILES.he0->GetChunkInfo(0);
			//	while (he0_header.offset < files::FILES.he0->size)
			//	{
			//		if (utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRS_CHUNK_ID) == 0 ||
			//			utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRC_CHUNK_ID) == 0 ||
			//			utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRF_CHUNK_ID) == 0 ||
			//			utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRN_CHUNK_ID) == 0 ||
			//			utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRT_CHUNK_ID) == 0 ||
			//			utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRM_CHUNK_ID) == 0 ||
			//			utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRR_CHUNK_ID) == 0
			//			)
			//		{
			//			chunk_reader::GENERIC_HE0_Chunk chunk;
			//			memcpy(&chunk, utils::add(files::FILES.he0->data, he0_header.offset), sizeof(chunk_reader::HumongousHeader) + sizeof(chunk.num_files));

			//			std::vector<pair> pairs;

			//			size_t pos = sizeof(chunk_reader::HumongousHeader) + sizeof(chunk.num_files);
			//			for (size_t j = 0; j < chunk.num_files; j++)
			//			{
			//				uint8_t rmim_index = *reinterpret_cast<uint8_t*>(utils::add(files::FILES.he0->data, he0_header.offset + pos));
			//				pairs.push_back({ rmim_index, 0, 0 });
			//				pos += sizeof(uint8_t);
			//			}

			//			for (size_t j = 0; j < chunk.num_files; j++)
			//			{
			//				uint32_t byte_pos = *reinterpret_cast<uint32_t*>(utils::add(files::FILES.he0->data, he0_header.offset + pos));
			//				pairs[j].actual_offset = pos;
			//				pairs[j].offset = byte_pos;
			//				pos += sizeof(uint32_t);
			//			}

			//			unsigned char* generic_data = reinterpret_cast<unsigned char*>(malloc(he0_header.ChunkSize()));
			//			size_t full_size = he0_header.ChunkSize();
			//			memcpy(generic_data, utils::add(files::FILES.he0->data, he0_header.offset), full_size);

			//			for (size_t j = 0; j < pairs.size(); j++)
			//			{
			//				if (pairs[j].rmim_offset != belongs_to_rmim)
			//					continue;

			//				size_t offset = pairs[j].offset + rmim_offsets[pairs[j].rmim_offset];
			//				if (offset > sl_instruction.scrp_offset)
			//				{
			//					uint32_t new_offset = pairs[j].offset + difference;
			//					memcpy(utils::add(generic_data, pairs[j].actual_offset), &new_offset, sizeof(uint32_t));
			//				}
			//			}

			//			files::FILES.he0->Replace(he0_header.offset, generic_data, he0_header.ChunkSize());
			//			free(generic_data);
			//		}
			//		else if (utils::chunkcmp(he0_header.chunk_id, chunk_reader::DLFL_CHUNK_ID) == 0)
			//		{
			//			chunk_reader::DLFL_Chunk dlfl_chunk;
			//			memcpy(&dlfl_chunk, utils::add(files::FILES.he0->data, he0_header.offset), sizeof(chunk_reader::HumongousHeader) + sizeof(dlfl_chunk.num_lflfs));

			//			std::vector<dlfl_offset> pairs;

			//			size_t pos_in_p = sizeof(chunk_reader::HumongousHeader) + sizeof(dlfl_chunk.num_lflfs);
			//			for (size_t j = 0; j < dlfl_chunk.num_lflfs; j++)
			//			{
			//				uint32_t byte_pos = *reinterpret_cast<uint32_t*>(utils::add(files::FILES.he0->data, he0_header.offset + pos_in_p));
			//				dlfl_offset p;
			//				p.actual_offset = pos_in_p;
			//				p.offset = byte_pos;

			//				pairs.push_back(p);
			//				pos_in_p += sizeof(uint32_t);
			//			}

			//			unsigned char* dlfl_data = reinterpret_cast<unsigned char*>(malloc(he0_header.ChunkSize()));
			//			memcpy(dlfl_data, utils::add(files::FILES.he0->data, he0_header.offset), he0_header.ChunkSize());

			//			for (size_t j = 0; j < pairs.size(); j++)
			//			{
			//				if (pairs[j].offset > sl_instruction.scrp_offset)
			//				{
			//					uint32_t new_offset = pairs[j].offset + difference;
			//					memcpy(utils::add(dlfl_data, pairs[j].actual_offset), &new_offset, sizeof(uint32_t));
			//				}
			//			}

			//			files::FILES.he0->Replace(he0_header.offset, dlfl_data, he0_header.ChunkSize());
			//			free(dlfl_data);
			//		}
			//		he0_header = files::FILES.he0->GetNextChunk(he0_header.offset);
			//	}
			//}

















			

			// Collect all rnims.
			std::vector<size_t> new_rmim_offsets;
			new_rmim_offsets.push_back(0);

			chunk_reader::ChunkInfo rmim_header = files::FILES.a->GetChunkInfo(0);
			while (rmim_header.offset < files::FILES.a->size)
			{
				if (utils::chunkcmp(rmim_header.chunk_id, chunk_reader::RMIM_CHUNK_ID) == 0)
					new_rmim_offsets.push_back(rmim_header.offset);
				chunk_reader::ChunkInfo next = files::FILES.a->GetNextChunk(rmim_header.offset);
				rmim_header = next;
			}

			std::map<std::string, std::vector<std::string>> chunks_in_index_pair =
			{
				{ chunk_reader::DIRS_CHUNK_ID, { chunk_reader::SCRP_CHUNK_ID } },
				{ chunk_reader::DIRC_CHUNK_ID, { chunk_reader::AKOS_CHUNK_ID } },
				{ chunk_reader::DIRF_CHUNK_ID, { chunk_reader::CHAR_CHUNK_ID } },
				{ chunk_reader::DIRN_CHUNK_ID, { chunk_reader::SOUN_CHUNK_ID, chunk_reader::TALK_CHUNK_ID, chunk_reader::DIGI_CHUNK_ID } },
				{ chunk_reader::DIRT_CHUNK_ID, { chunk_reader::TLKE_CHUNK_ID } },
				{ chunk_reader::DIRM_CHUNK_ID, { chunk_reader::MULT_CHUNK_ID, chunk_reader::AWIZ_CHUNK_ID } },
				{ chunk_reader::DIRR_CHUNK_ID, { chunk_reader::RMDA_CHUNK_ID } },
			};

			chunk_reader::ChunkInfo he0_header = files::FILES.he0->GetChunkInfo(0);
			while (he0_header.offset < files::FILES.he0->size)
			{
				if (utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRS_CHUNK_ID) == 0 ||
					utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRC_CHUNK_ID) == 0 ||
					utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRF_CHUNK_ID) == 0 ||
					utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRN_CHUNK_ID) == 0 ||
					utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRT_CHUNK_ID) == 0 ||
					utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRM_CHUNK_ID) == 0 ||
					utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRR_CHUNK_ID) == 0
					)
				{
					std::vector<std::string> chunk_ids;

					chunk_reader::GENERIC_HE0_Chunk chunk;
					memcpy(&chunk, utils::add(files::FILES.he0->data, he0_header.offset), sizeof(chunk_reader::HumongousHeader) + sizeof(chunk.num_files));

					std::vector<pair> pairs;

					size_t pos = sizeof(chunk_reader::HumongousHeader) + sizeof(chunk.num_files);
					for (size_t j = 0; j < chunk.num_files; j++)
					{
						uint8_t rmim_index = *reinterpret_cast<uint8_t*>(utils::add(files::FILES.he0->data, he0_header.offset + pos));
						pairs.push_back({ rmim_index, 0, 0 });
						pos += sizeof(uint8_t);
					}

					for (size_t j = 0; j < chunk.num_files; j++)
					{
						uint32_t byte_pos = *reinterpret_cast<uint32_t*>(utils::add(files::FILES.he0->data, he0_header.offset + pos));
						pairs[j].actual_offset = pos;
						pairs[j].offset = byte_pos;
						pos += sizeof(uint32_t);
					}

					for (size_t j = 0; j < pairs.size(); j++)
					{
						if (pairs[j].offset == 0)
							continue;

						chunk_reader::ChunkInfo chunk_info = files::FILES.a->GetChunkInfo(new_rmim_offsets[pairs[j].rmim_offset] + pairs[j].offset);
						std::vector<std::string>& index_chunks = chunks_in_index_pair[std::string(reinterpret_cast<char*>(chunk.chunk_id))];
						bool found = false;
						for (size_t k = 0; k < index_chunks.size(); k++)
							if (utils::chunkcmp(chunk_info.chunk_id, index_chunks[k].c_str()) == 0)
								found = true;

						assert(found);
					}
				}
				else if (utils::chunkcmp(he0_header.chunk_id, chunk_reader::DLFL_CHUNK_ID) == 0)
				{
					chunk_reader::DLFL_Chunk dlfl_chunk;
					memcpy(&dlfl_chunk, utils::add(files::FILES.he0->data, he0_header.offset), sizeof(chunk_reader::HumongousHeader) + sizeof(dlfl_chunk.num_lflfs));

					std::vector<dlfl_offset> pairs;

					size_t pos_in_p = sizeof(chunk_reader::HumongousHeader) + sizeof(dlfl_chunk.num_lflfs);
					for (size_t j = 0; j < dlfl_chunk.num_lflfs; j++)
					{
						uint32_t byte_pos = *reinterpret_cast<uint32_t*>(utils::add(files::FILES.he0->data, he0_header.offset + pos_in_p));
						dlfl_offset p;
						p.actual_offset = pos_in_p;
						p.offset = byte_pos;

						pairs.push_back(p);
						pos_in_p += sizeof(uint32_t);
					}

					for (size_t j = 0; j < pairs.size(); j++)
					{
						if (pairs[j].offset == 0)
							continue;

						chunk_reader::ChunkInfo chunk_info = files::FILES.a->GetChunkInfo(pairs[j].offset);
						assert(utils::chunkcmp(chunk_info.chunk_id, chunk_reader::RMIM_CHUNK_ID) == 0);
					}
				}
				he0_header = files::FILES.he0->GetNextChunk(he0_header.offset);
			}

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];

			form->entryView->Nodes->Clear();
			form->tabControl1->Controls->Clear();

			HumongousFileEditor::chunk_reader::ResourceGatherer rg;
			rg.Read(files::FILES.a);

			return true;
		}
		return false;
	}

	bool TALKTab::GetData(chunk_reader::FileContainer*& fc, size_t offset, chunk_reader::SDAT_Chunk& sdat_chunk, chunk_reader::HSHD_Chunk& hshd_chunk)
	{
		std::vector<chunk_reader::ChunkInfo> children = fc->GetChildren(offset);
		size_t hshd_offset = -1;
		size_t sdat_offset = -1;
		for (size_t i = 0; i < children.size(); i++)
		{
			if (utils::chunkcmp(children[i].chunk_id, chunk_reader::HSHD_CHUNK_ID) == 0)
				hshd_offset = children[i].offset;
			if (utils::chunkcmp(children[i].chunk_id, chunk_reader::SDAT_CHUNK_ID) == 0)
				sdat_offset = children[i].offset;
		}

		if (hshd_offset == -1)
			return false;

		if (sdat_offset == -1)
			return false;

		memcpy(&hshd_chunk, utils::add(fc->data, hshd_offset), sizeof(chunk_reader::HSHD_Chunk));

		size_t header_size = sizeof(chunk_reader::SDAT_Chunk) - sizeof(sdat_chunk.data); // Pointer in the SDAT class is size 8 and needs to be deducted.
		memcpy(&sdat_chunk, utils::add(fc->data, sdat_offset), header_size);
		sdat_chunk.data = utils::add(fc->data, sdat_offset + header_size);

		return true;
	}
}