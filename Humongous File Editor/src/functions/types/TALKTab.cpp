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

				ofn.lpstrFilter = L"Humongous Talkie Files (*.HE2)\
						\0*.HE2;*.he2\0";
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
				{
					return false;
				}
			}

			if (!utils::ends_with(path, ".wav"))
				path += ".wav";

			size_t wave_size = 0;
			if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::FTell(path.c_str(), wave_size)))
				return false;

			void* chunk_alloc = malloc(wave_size);
			uaudio::wave_reader::ChunkCollection chunkCollection(chunk_alloc, wave_size);
			if (UAUDIOWAVEREADERFAILED(uaudio::wave_reader::WaveReader::LoadWave(path.c_str(), chunkCollection)))
				return false;

			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID)))
				return false;

			if (UAUDIOWAVEREADERFAILED(chunkCollection.GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID)))
				return false;

			if (fmt_chunk.byteRate != 11025)
				return false;

			if (fmt_chunk.sampleRate != 11025)
				return false;

			if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8)
				return false;

			if (fmt_chunk.numChannels != uaudio::wave_reader::WAVE_CHANNELS_MONO)
				return false;

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

			std::vector<talk_instruction> total_instructions;

			chunk_reader::ChunkInfo header = files::FILES.a->GetChunkInfo(0);
			while (header.offset < files::FILES.a->size)
			{
				if (utils::chunkcmp(header.chunk_id, chunk_reader::LSCR_CHUNK_ID) == 0 ||
					utils::chunkcmp(header.chunk_id, chunk_reader::LSC2_CHUNK_ID) == 0
					)
				{
					std::vector<talk_instruction> instructions;
					if (SCRPTab::GetData(files::FILES.a, header.offset, instructions))
					{
						for (size_t k = 0; k < instructions.size(); k++)
						{
							talk_instruction& instr = instructions[k];

							uint32_t full_size = talk_chunk.ChunkSize();
							size_t new_offset = instr.talk_offset - dif_size;
							if (instr.talk_offset >= offset)
							{
								if (std::to_string(instr.talk_offset).size() != std::to_string(new_offset).size())
									assert(false);
								if (instr.talk_offset == offset &&
									std::to_string(instr.talk_size).size() != std::to_string(full_size).size())
									assert(false);
								total_instructions.push_back(instr);
							}
						}
					}
				}
				chunk_reader::ChunkInfo next = files::FILES.a->GetNextChunk(header.offset);
				header = next;
			}

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
			form->entryView->Nodes->Clear();
			form->tabControl1->Controls->Clear();

			unsigned char* full_data = reinterpret_cast<unsigned char*>(malloc(files::FILES.a->size));
			memcpy(full_data, files::FILES.a->data, files::FILES.a->size);
			for (int g = total_instructions.size(); g-- > 0; )
			{
				talk_instruction& instruction = total_instructions[g];
				size_t new_offset = instruction.talk_offset + dif_size;
				std::string offset_str = std::to_string(new_offset);
				memcpy(utils::add(full_data, instruction.talk_offset_pos), offset_str.c_str(), offset_str.size());

				std::string talk_size_str = std::to_string(talk_chunk.ChunkSize());
				memcpy(utils::add(full_data, instruction.talk_size_pos), talk_size_str.c_str(), talk_size_str.size());
			}
			files::FILES.a->Replace(0, full_data, files::FILES.a->size);

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