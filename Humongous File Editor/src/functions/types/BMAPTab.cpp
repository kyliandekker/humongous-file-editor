#include "functions/types/BMAPTab.h"

#include <WTypesbase.h>
#include <commdlg.h>

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/HumongousChunks.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"
#include "functions/TabFunctions.h"
#include "functions/ChunkFunctions.h"
#include <HumongousEditorForm.h>

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

	struct offset_pair
	{
		size_t start, end;
	};

	struct strip
	{
		unsigned char* data;
		size_t size;
	};

	struct IndexColor
	{
		uint8_t index;
		uint8_t trans_color;

		IndexColor(uint8_t index, uint8_t trans_color)
		{
			this->index = index;
			this->trans_color = trans_color;
		}

		IndexColor()
		{}
	};

	bool BMAPTab::GetDataBMAP(chunk_reader::FileContainer*&, chunk_reader::BMAP_Chunk& bmap_chunk, chunk_reader::APAL_Chunk& apal_chunk, uint8_t fill_color, size_t width, size_t height, img_info& info)
	{
		if (utils::chunkcmp(bmap_chunk.chunk_id, chunk_reader::BMAP_CHUNK_ID) != 0)
		{
			return false;
		}

		if (utils::chunkcmp(apal_chunk.chunk_id, chunk_reader::APAL_CHUNK_ID) != 0)
		{
			return false;
		}

		size_t header_size = sizeof(chunk_reader::BMAP_Chunk) - sizeof(bmap_chunk.data); // Pointer in the BMAP class is size 8 and needs to be deducted.
		size_t bmap_size = bmap_chunk.ChunkSize() - header_size;

		int palen = bmap_chunk.encoding % 10;

		bool he_transparent = bmap_chunk.encoding >= 0x90 && bmap_chunk.encoding <= 0x94;

		if (!DecodeHE(fill_color, bmap_chunk.data, bmap_size, width, height, palen, he_transparent, info))
		{
			return false;
		}

		std::vector<uint8_t> newOut;
		for (size_t i = 0; i < info.size; i++)
		{
			newOut.push_back(apal_chunk.data[info.data.data[i] * 3]);
			newOut.push_back(apal_chunk.data[info.data.data[i] * 3 + 1]);
			newOut.push_back(apal_chunk.data[info.data.data[i] * 3 + 2]);
			if (info.data.data[i] == fill_color)
			{
				newOut.push_back(0);
			}
			else
			{
				newOut.push_back(255);
			}
		}

		info.size = newOut.size();
		info.data = ImageData(newOut.size(), newOut.data());
		info.channels = 4;

		return true;
	}

	bool BMAPTab::GetDataSMAP(chunk_reader::FileContainer*&, size_t width, size_t height, chunk_reader::SMAP_Chunk& smap_chunk, chunk_reader::APAL_Chunk& apal_chunk, img_info& info)
	{
		size_t header_size = sizeof(chunk_reader::SMAP_Chunk) - sizeof(smap_chunk.data); // Pointer in the SMAP class is size 8 and needs to be deducted.
		size_t smap_size = smap_chunk.ChunkSize() - header_size;

		uint32_t strip_width = 8;
		size_t num_strips = static_cast<size_t>(static_cast<size_t>(floor(static_cast<double>(width / strip_width))));

		std::vector<uint32_t> offsets;
		int j = 0;
		for (size_t i = 0; i < num_strips; i++, j += sizeof(uint32_t))
		{
			uint32_t number = *reinterpret_cast<uint32_t*>(utils::add(smap_chunk.data, j));
			number -= sizeof(chunk_reader::HumongousHeader);
			offsets.push_back(number);
		}

		std::vector<offset_pair> index;

		for (size_t i = 0; i < offsets.size(); i++)
		{
			index.push_back({ offsets[i], (i + 1) == offsets.size() ? smap_size : offsets[i + 1] });
		}

		std::vector<strip> strips;
		for (size_t i = 0; i < num_strips; i++)
		{
			strips.push_back({ utils::add(smap_chunk.data, index[i].start), index[i].end - index[i].start });
		}

		size_t total_size = 0;
		std::vector< std::vector<std::vector<IndexColor>>> data_blocks;
		for (size_t i = 0; i < strips.size(); i++)
		{
			strip& strip = strips[i];

			std::vector<std::vector<IndexColor>> data_new_block;

			uint8_t code = strip.data[0];

			bool horizontal = true;
			if (code >= 0x03 && code <= 0x12 || code >= 0x22 && code <= 0x26)
			{
				horizontal = false;
			}

			bool he_transparent = code >= 0x22 && code <= 0x30 || code >= 0x54 && code <= 0x80 || code >= 0x8F;

			int palen = code % 10;

			uint8_t color = strip.data[1];

			img_info strip_info;
			if (code >= 0x40 && code <= 0x80)
			{
				if (!DecodeMajmin(color, utils::add(strip.data, 2), strip.size - 2, strip_width, height, palen, he_transparent, strip_info))
				{
					return false;
				}
			}
			else if (code >= 0x0E && code <= 0x30)
			{
				if (!DecodeBasic(color, utils::add(strip.data, 2), strip.size - 2, strip_width, height, palen, he_transparent, strip_info))
				{
					return false;
				}
			}
			else if (code >= 0x86 && code <= 0x94)
			{
				if (!DecodeHE(color, utils::add(strip.data, 2), strip.size - 2, strip_width, height, palen, he_transparent, strip_info))
				{
					return false;
				}
			}
			else if (code >= 0x01 && code <= 0x95)
			{
				if (!DecodeRaw(strip.data, strip.size, strip_width, height, palen, he_transparent, strip_info))
				{
					return false;
				}
			}

			ImageData new_data = ImageData(strip_info.data.size, strip_info.data.data);

			if (!horizontal)
			{
				uint32_t dataIndex = 0;
				for (uint32_t k = 0; k < strip_width; ++k)
				{
					for (uint32_t l = 0; l < static_cast<uint32_t>(height); ++l)
					{
						new_data.data[(l * strip_width) + k] = strip_info.data.data[dataIndex];
						++dataIndex;
					}
				}
			}

			strip_info.data = new_data;
			total_size += strip_info.size;

			for (size_t k = 0; k < height; k++)
			{
				std::vector<IndexColor> new_strip;
				for (size_t b = 0; b < strip_width; b++)
				{
					new_strip.push_back({ strip_info.data.data[(k * strip_width) + b], color });
				}
				data_new_block.push_back(new_strip);
			}

			data_blocks.push_back(data_new_block);
		}

		std::vector<std::vector<IndexColor>> finals;

		for (size_t i = 0; i < data_blocks[0].size(); i++)
		{
			std::vector<IndexColor> arr;
			for (size_t b = 0; b < data_blocks.size(); b++)
			{
				for (size_t h = 0; h < strip_width; h++)
				{
					arr.push_back(data_blocks[b][i][h]);
				}
			}
			finals.push_back(arr);
		}

		std::vector<IndexColor> final_data = std::vector<IndexColor>(total_size);
		int pos = 0;
		for (size_t i = 0; i < finals.size(); i++)
		{
			std::vector<IndexColor>& arr = finals[i];
			for (size_t b = 0; b < arr.size(); b++)
			{
				final_data[pos] = finals[i][b];
				pos++;
			}
		}

		info.width = width;
		info.height = height;
		info.size = total_size;
		info.channels = 4;

		std::vector<uint8_t> newOut;
		for (size_t i = 0; i < info.size; i++)
		{
			newOut.push_back(apal_chunk.data[final_data[i].index * 3]);
			newOut.push_back(apal_chunk.data[final_data[i].index * 3 + 1]);
			newOut.push_back(apal_chunk.data[final_data[i].index * 3 + 2]);
			if (final_data[i].index == final_data[i].trans_color)
			{
				newOut.push_back(255);
			}
			else
			{
				newOut.push_back(255);
			}
		}

		info.size = newOut.size();
		info.data = ImageData(newOut.size(), newOut.data());

		return true;
	}

	// TODO: REMOVE
	bool BMAPTab::ReplacePalette(chunk_reader::FileContainer*& fc, size_t offset)
	{
		OPENFILENAME ofn;
		TCHAR sz_file[260] = { 0 };

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = sz_file;
		ofn.nMaxFile = sizeof(sz_file);
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileNameW(&ofn))
		{
			const auto path = new char[wcslen(ofn.lpstrFile) + 1];
			wsprintfA(path, "%S", ofn.lpstrFile);

			std::string file_path = std::string(path);

			FILE* palette_data_file = nullptr;
			fopen_s(&palette_data_file, file_path.c_str(), "rb");

			// Go to eof and store the size.
			fseek(palette_data_file, 0, SEEK_END);
			size_t filesize = ftell(palette_data_file);
			rewind(palette_data_file);

			TempAllocator palette_data = TempAllocator(filesize);
			fread(palette_data.data, filesize, 1, palette_data_file);

			fc->Replace(offset, palette_data.data, filesize);

			return true;
		}
		return false;
	}

	// TODO: REMOVE
	bool BMAPTab::ReplaceBmap(chunk_reader::FileContainer*& fc, size_t offset)
	{
		OPENFILENAME ofn;
		TCHAR sz_file[260] = { 0 };

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = sz_file;
		ofn.nMaxFile = sizeof(sz_file);
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileNameW(&ofn))
		{
			const auto path = new char[wcslen(ofn.lpstrFile) + 1];
			wsprintfA(path, "%S", ofn.lpstrFile);

			std::string file_path = std::string(path);

			FILE* bmap_data_file = nullptr;
			fopen_s(&bmap_data_file, file_path.c_str(), "rb");

			// Go to eof and store the size.
			fseek(bmap_data_file, 0, SEEK_END);
			size_t filesize = ftell(bmap_data_file);
			rewind(bmap_data_file);

			TempAllocator bmap_data = TempAllocator(filesize);
			fread(bmap_data.data, filesize, 1, bmap_data_file);

			fc->Replace(offset, bmap_data.data, filesize);

			HumongousEditorForm^ form = (HumongousEditorForm^)Application::OpenForms["HumongousEditorForm"];
			form->entryView->Nodes->Clear();
			form->tabControl1->Controls->Clear();

			HumongousFileEditor::chunk_reader::ResourceGatherer rg;
			rg.Read(files::FILES.a);

			// Collect all rnims.
			std::vector<chunk_reader::ChunkInfo> rmim_offsets;
			rmim_offsets.push_back(chunk_reader::ChunkInfo());

			chunk_reader::ChunkInfo rmim_header = files::FILES.a->GetChunkInfo(0);
			while (rmim_header.offset < files::FILES.a->size)
			{
				if (utils::chunkcmp(rmim_header.chunk_id, chunk_reader::RMIM_CHUNK_ID) == 0)
					rmim_offsets.push_back(rmim_header);
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

			{
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

						std::map<uint8_t, uint32_t> rmims;
						for (size_t j = 0; j < rmim_offsets.size(); j++)
							rmims[j] = 0;

						unsigned char* generic_data = reinterpret_cast<unsigned char*>(malloc(he0_header.ChunkSize()));
						memcpy(generic_data, utils::add(files::FILES.he0->data, he0_header.offset), he0_header.ChunkSize());

						chunk_reader::ChunkInfo mult;
						mult.SetChunkSize(0);
						for (size_t j = 0; j < pairs.size(); j++)
						{
							if (pairs[j].offset == 0)
								continue;

							std::vector<std::string>& index_chunks = chunks_in_index_pair[std::string(reinterpret_cast<char*>(chunk.chunk_id))];

							chunk_reader::ChunkInfo a_header = files::FILES.a->GetChunkInfo(rmim_offsets[pairs[j].rmim_offset].offset);
							int32_t num = -1;
							while (num != rmims[pairs[j].rmim_offset] && a_header.offset < files::FILES.a->size)
							{
								for (size_t k = 0; k < index_chunks.size(); k++)
									if (utils::chunkcmp(a_header.chunk_id, index_chunks[k].c_str()) == 0)
									{
										if (utils::chunkcmp(he0_header.chunk_id, chunk_reader::DIRM_CHUNK_ID) != 0)
										{
											num++;
										}
										else
										{
											if (utils::chunkcmp(a_header.chunk_id, chunk_reader::MULT_CHUNK_ID) == 0)
											{
												mult = a_header;
												num++;
											}
											else
											{
												if (a_header.offset > mult.offset && a_header.offset > mult.offset + mult.ChunkSize())
													num++;
											}
										}
									}
								if (num != rmims[pairs[j].rmim_offset])
									a_header = files::FILES.a->GetNextChunk(a_header.offset);
							}

							rmims[pairs[j].rmim_offset] = num + 1;

							pairs[j].offset = a_header.offset - rmim_offsets[pairs[j].rmim_offset].offset;
							memcpy(utils::add(generic_data, pairs[j].actual_offset), &pairs[j].offset, sizeof(uint32_t));
						}

						files::FILES.he0->Replace(he0_header.offset, generic_data, he0_header.ChunkSize());
						free(generic_data);
					}
					he0_header = files::FILES.he0->GetNextChunk(he0_header.offset);
				}
			}

			{
				size_t dlfl_offst = -1;

				chunk_reader::ChunkInfo he0_header = files::FILES.he0->GetChunkInfo(0);
				while (he0_header.offset < files::FILES.he0->size)
				{
					if (utils::chunkcmp(he0_header.chunk_id, chunk_reader::DLFL_CHUNK_ID) == 0)
					{
						dlfl_offst = he0_header.offset;
						break;
					}
					he0_header = files::FILES.he0->GetNextChunk(he0_header.offset);
				}

				chunk_reader::DLFL_Chunk dlfl_chunk;
				memcpy(&dlfl_chunk, utils::add(files::FILES.he0->data, dlfl_offst), sizeof(chunk_reader::HumongousHeader) + sizeof(dlfl_chunk.num_lflfs));

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

				unsigned char* dlfl_data = reinterpret_cast<unsigned char*>(malloc(he0_header.ChunkSize()));
				memcpy(dlfl_data, utils::add(files::FILES.he0->data, he0_header.offset), he0_header.ChunkSize());

				for (size_t j = 0; j < pairs.size(); j++)
					memcpy(utils::add(dlfl_data, pairs[j].actual_offset), &rmim_offsets[j].offset, sizeof(uint32_t));

				files::FILES.he0->Replace(he0_header.offset, dlfl_data, he0_header.ChunkSize());
				free(dlfl_data);
			}

			return true;
		}
		return false;
	}
}