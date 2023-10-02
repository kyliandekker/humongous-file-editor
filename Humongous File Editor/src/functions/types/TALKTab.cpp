#include "functions/types/TALKTab.h"

#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/UAUDIO_WAVE_READER_RESULT.h>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <wtypes.h>
#include <commdlg.h>
#include <HumongousEditorForm.h>

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "functions/ChunkFunctions.h"
#include "functions/ResourceGatherer.h"
#include "functions/types/SCRPTab.h"
#include <cassert>

namespace HumongousFileEditor
{
	bool TALKTab::ReplaceResource(chunk_reader::FileContainer*& fc, size_t offset)
	{
		std::string path;
		uaudio::wave_reader::DATA_Chunk data_chunk;
		uaudio::wave_reader::FMT_Chunk fmt_chunk;
		if (SoundTab::ReplaceResource(path, fmt_chunk, data_chunk))
		{
			if (files::FILES.he0 == nullptr)
			{
				OPENFILENAME ofn;
				TCHAR sz_file[260] = { 0 };

				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = sz_file;
				ofn.nMaxFile = sizeof(sz_file);

				ofn.lpstrFilter = L"Humongous Index Files (*.HE0)\
						\0*.HE0;*.he0\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = nullptr;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
				if (GetOpenFileNameW(&ofn))
				{
					const auto path = new char[wcslen(ofn.lpstrFile) + 1];
					wsprintfA(path, "%S", ofn.lpstrFile);

					chunk_reader::ResourceGatherer f = chunk_reader::ResourceGatherer();
					if (f.Read(path))
					{
						System::Windows::Forms::MessageBox::Show("Successfully opened file.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
						form->toolProgressBar->Value = 0;
						return false;
					}

					delete[] path;
				}
				else
					return false;
			}

			if (!utils::ends_with(path, ".wav"))
				path += ".wav";

			size_t wave_size = 0;
			if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::FTell(path.c_str(), wave_size)))
				return false;

			void* chunk_alloc = malloc(wave_size);
			uaudio::wave_reader::ChunkCollection chunkCollection(chunk_alloc, wave_size);
			if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::LoadWave(path.c_str(), chunkCollection)))
			{
				free(chunk_alloc);
				return false;
			}

			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
			{
				free(chunk_alloc);
				return false;
			}

			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
			{
				free(chunk_alloc);
				return false;
			}

			if (fmt_chunk.byteRate != 11025)
			{
				free(chunk_alloc);
				return false;
			}

			if (fmt_chunk.sampleRate != 11025)
			{
				free(chunk_alloc);
				return false;
			}

			if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8)
			{
				free(chunk_alloc);
				return false;
			}

			if (fmt_chunk.numChannels != uaudio::wave_reader::WAVE_CHANNELS_MONO)
			{
				free(chunk_alloc);
				return false;
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
				return false;
			}

			if (hshd_offset == -1)
			{
				free(chunk_alloc);
				return false;
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

			chunk_reader::ChunkInfo oldChunk = fc->GetChunkInfo(offset);
			
			fc->Replace(talk_offset, new_data, talk_chunk.ChunkSize());

			int32_t dif_size = talk_chunk.ChunkSize() - oldChunk.ChunkSize();

			free(chunk_alloc);
			free(new_data);

			unsigned char* full_data = reinterpret_cast<unsigned char*>(malloc(files::FILES.a->size));
			memcpy(full_data, files::FILES.a->data, files::FILES.a->size);

			std::string talk_size_str = std::to_string(talk_chunk.ChunkSize());

			std::vector<talk_instruction> shorter_longer_instructions;

			chunk_reader::ChunkInfo header = files::FILES.a->GetChunkInfo(0);
			while (header.offset < files::FILES.a->size)
			{
				if (utils::chunkcmp(header.chunk_id, chunk_reader::LSCR_CHUNK_ID) == 0 ||
					utils::chunkcmp(header.chunk_id, chunk_reader::LSC2_CHUNK_ID) == 0 ||
					utils::chunkcmp(header.chunk_id, chunk_reader::SCRP_CHUNK_ID) == 0
					)
				{
					std::vector<talk_instruction> instructions;
					if (SCRPTab::GetData(files::FILES.a, header.offset, instructions))
					{
						for (size_t k = 0; k < instructions.size(); k++)
						{
							talk_instruction& instruction = instructions[k];

							size_t new_offset = instruction.talk_offset + dif_size;
							if (instruction.talk_offset >= offset)
							{
								// If the new offset of this instruction is not the same as the previous one.
								if (std::to_string(instruction.talk_offset).size() != std::to_string(new_offset).size())
									shorter_longer_instructions.push_back(instruction);
								// If the size of the changed talk chunk is now shorter or longer.
								else if (instruction.talk_offset == offset && std::to_string(instruction.talk_size).size() != talk_size_str.size())
									shorter_longer_instructions.push_back(instruction);
								// Else, simply replace it. No need to move things around in the (a) and HE0.
								else
								{
									if (instruction.talk_offset == offset)
										memcpy(utils::add(full_data, instruction.talk_size_pos), talk_size_str.c_str(), talk_size_str.size());
									else
									{
										std::string offset_str = std::to_string(new_offset);
										memcpy(utils::add(full_data, instruction.talk_offset_pos), offset_str.c_str(), offset_str.size());
									}
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

			for (size_t i = shorter_longer_instructions.size(); i-- > 0; )
			{
				talk_instruction& instruction = shorter_longer_instructions[i];

				int32_t difference = 0;

				size_t replace_on_offset = 0;
				size_t old_replace_value = 0;
				size_t replace_value = 0;

				size_t new_offset = instruction.talk_offset + dif_size;
				if (std::to_string(instruction.talk_offset).size() != std::to_string(new_offset).size())
				{
					difference = static_cast<int32_t>(std::to_string(new_offset).size()) - static_cast<int32_t>(std::to_string(instruction.talk_offset).size());

					replace_on_offset = instruction.talk_offset_pos - instruction.scrp_offset;
					old_replace_value = instruction.talk_offset;
					replace_value = new_offset;
				}
				else if (instruction.talk_offset == offset && std::to_string(instruction.talk_size).size() != talk_size_str.size())
				{
					difference = static_cast<int32_t>(talk_size_str.size()) - static_cast<int32_t>(std::to_string(instruction.talk_size).size());

					replace_on_offset = instruction.talk_size_pos - instruction.scrp_offset;
					old_replace_value = instruction.talk_size;
					replace_value = talk_chunk.ChunkSize();
				}

				size_t new_size = instruction.scrp_size + difference;
				unsigned char* scrp_data = reinterpret_cast<unsigned char*>(malloc(new_size));
				if (!scrp_data)
					continue;

				ZeroMemory(scrp_data, new_size);

				// Copy everything until the replace point.
				memcpy(scrp_data, utils::add(files::FILES.a->data, instruction.scrp_offset), replace_on_offset);
				chunk_reader::ChunkInfo chunkInfo = files::FILES.a->GetChunkInfo(instruction.scrp_offset);
				chunkInfo.SetChunkSize(new_size);
				memcpy(
					scrp_data,
					&chunkInfo,
					sizeof(chunk_reader::ChunkInfo)
				);

				// Copy the replace value.
				std::string replace_value_str = std::to_string(replace_value);
				memcpy(
					utils::add(scrp_data, replace_on_offset),
					replace_value_str.c_str(),
					replace_value_str.size()
				);

				// Copy the rest of the data.
				int rest_data_place_pos = replace_on_offset + replace_value_str.size();
				int rest_data_pos = instruction.scrp_offset + replace_on_offset + std::to_string(old_replace_value).size();
				int rest_data_size = instruction.scrp_size - rest_data_place_pos + difference;
				memcpy(
					utils::add(scrp_data, rest_data_place_pos),
					utils::add(files::FILES.a->data, rest_data_pos),
					rest_data_size
				);

				files::FILES.a->Replace(instruction.scrp_offset, scrp_data, new_size);
				free(scrp_data);

				header = files::FILES.he0->GetChunkInfo(0);
				int random_number_for_unique_id = 0;
				while (header.offset < files::FILES.he0->size)
				{
					if (
						utils::chunkcmp(header.chunk_id, chunk_reader::RNAM_CHUNK_ID) == 0 ||
						utils::chunkcmp(header.chunk_id, chunk_reader::DIRI_CHUNK_ID) == 0 ||
						utils::chunkcmp(header.chunk_id, chunk_reader::DIRS_CHUNK_ID) == 0 ||
						utils::chunkcmp(header.chunk_id, chunk_reader::DIRC_CHUNK_ID) == 0 ||
						utils::chunkcmp(header.chunk_id, chunk_reader::DIRF_CHUNK_ID) == 0 ||
						utils::chunkcmp(header.chunk_id, chunk_reader::DIRN_CHUNK_ID) == 0 ||
						utils::chunkcmp(header.chunk_id, chunk_reader::DIRT_CHUNK_ID) == 0 ||
						utils::chunkcmp(header.chunk_id, chunk_reader::DIRM_CHUNK_ID) == 0 ||
						utils::chunkcmp(header.chunk_id, chunk_reader::DIRR_CHUNK_ID) == 0 ||
						utils::chunkcmp(header.chunk_id, chunk_reader::DISK_CHUNK_ID) == 0 ||
						utils::chunkcmp(header.chunk_id, chunk_reader::DLFL_CHUNK_ID) == 0
						)
					{
						chunk_reader::GENERIC_HE0_Chunk chunk;
						memcpy(&chunk, utils::add(files::FILES.he0->data, header.offset), sizeof(chunk_reader::HumongousHeader) + sizeof(chunk.num_files));
						chunk.data = utils::add(files::FILES.he0->data, header.offset + sizeof(chunk_reader::HumongousHeader) + sizeof(chunk.num_files));

						size_t pos = sizeof(chunk_reader::HumongousHeader) + sizeof(chunk.num_files) + chunk.num_files;
						while (pos < chunk.ChunkSize())
						{
							uint32_t byte_pos = *reinterpret_cast<uint32_t*>(utils::add(files::FILES.he0->data, header.offset + pos));
							if (byte_pos >= instruction.scrp_offset)
							{
								byte_pos += difference;
								memcpy(utils::add(files::FILES.he0->data, header.offset + pos), &byte_pos, sizeof(uint32_t));
							}
							pos += sizeof(uint32_t);
						}
					}
					header = files::FILES.he0->GetNextChunk(header.offset);
				}
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