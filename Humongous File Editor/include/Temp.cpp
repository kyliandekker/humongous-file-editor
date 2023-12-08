#include "Temp.h"

#include <assert.h>

#include "lowlevel/FileContainer.h"
#include "HumongousEditorForm.h"
#include "lowlevel/utils.h"
#include "lowlevel/HumongousChunks.h"
#include <vector>
#include <map>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		void Temp::AddEntry(uint32_t, std::string)
		{
		}

		void Temp::Read(const char* )
		{
			//FileContainer fc = FileContainer(path);

			////// START TEST (ROOT).
			////Node n1 = fc.Start();
			////assert(n1.ChunkSize() == 114231949);

			////// CHUNK RIGHT AFTER ROOT TEST.
			////Node n2 = n1.Child();
			////assert(n2.ChunkSize() == 50554);

			////// CHUNK AFTER THE N2.
			////Node n3 = n2.Next();
			////assert(n3.ChunkSize() == 63886);

			////// CHUNK BEFORE N3 (WHICH SHOULD BE N2).
			////Node n4 = n3.Previous();
			////assert(n4.ChunkSize() == n2.ChunkSize());

			////// GETTING CHUNK VIA OFFSET (WHICH SHOULD BE N2).
			////Node n5 = fc.NodeFromOffset(n2.offset);
			////assert(n5.ChunkSize() == n2.ChunkSize());

			////// EOF.
			////Node n6 = n1.Next();
			////assert(n6.null);

			////// BOF.
			////Node n7 = n1.Previous();
			////assert(n1.ChunkSize() == n7.ChunkSize());

			//FILE* file = nullptr;
			//fopen_s(&file, "D:/ekkes/test.txt", "wb");
			//if (file == nullptr)
			//{
			//	printf("Cannot open file");
			//	return;
			//}

			//auto getPrintText = [](unsigned char* chunk_id, uint32_t offset, uint32_t total_size, bool close = true)
			//{
			//	std::string chunk_id_name = std::string(reinterpret_cast<char*>(chunk_id));
			//	chunk_id_name.resize(CHUNK_ID_SIZE);

			//	std::string text = "<";
			//	text += chunk_id_name;
			//	text += " position=\"";
			//	text += std::to_string(offset);
			//	text += "\"";
			//	text += " total_size=\"";
			//	text += std::to_string(total_size);
			//	text += "\"";
			//	text += " size=\"";
			//	text += std::to_string(total_size - sizeof(HumongousHeader));
			//	text += "\"";
			//	if (close)
			//		text += " /";
			//	text += ">\n";
			//	return text;
			//};

			//Node root = fc.Start();
			//Node n1 = root;

			//struct high_chunk
			//{
			//	uint32_t offset;
			//	unsigned char chunk_id[CHUNK_ID_SIZE] = {};
			//};

			//std::vector<high_chunk> offsets;

			//std::string full_text;

			//std::map<std::string, uint32_t> entries;

			//uint32_t i = 0;
			//while (i < 99999)
			//{
			//	uint32_t prev_offset = n1.offset + n1.ChunkSize();
			//	unsigned char prev_chunk_id[CHUNK_ID_SIZE] = {};
			//	memcpy(prev_chunk_id, n1.chunk_id, CHUNK_ID_SIZE);

			//	if (n1.offset == fc.size)
			//		break;

			//	std::string chunk_id_name = std::string(reinterpret_cast<char*>(n1.chunk_id));
			//	chunk_id_name.resize(CHUNK_ID_SIZE);
			//	entries[chunk_id_name]++;

			//	n1 = n1.Next();

			//	if (n1.ChunkSize() + n1.offset < prev_offset)
			//	{
			//		high_chunk h = {};
			//		memcpy(h.chunk_id, prev_chunk_id, CHUNK_ID_SIZE);
			//		h.offset = prev_offset;
			//		offsets.push_back(h);
			//	}

			//	if (file && n1.offset < fc.size)
			//	{
			//		for (size_t i = 0; i < offsets.size(); i++)
			//			full_text += "\t";
			//		std::string text = getPrintText(n1.chunk_id, n1.offset, n1.ChunkSize(), n1.Next().offset >= n1.offset + n1.ChunkSize());
			//		
			//		full_text += text;
			//	}
			//	
			//	for (size_t i = offsets.size() - 1; i != (size_t)-1; i--)
			//		if (offsets[i].offset <= n1.Next().offset)
			//		{
			//			for (size_t j = 0; j < i; j++)
			//				full_text += "\t";
			//			std::string text = "</" + std::string(std::begin(offsets[i].chunk_id), std::end(offsets[i].chunk_id)) + ">\n";

			//			full_text += text;
			//			offsets.erase(offsets.begin() + i);
			//			break;
			//		}

			//	if (utils::chunkcmp(n1.chunk_id, TALK_CHUNK_ID) == 0)
			//		AddEntry(n1.offset, ".wav");
			//	else if (utils::chunkcmp(n1.chunk_id, SGEN_CHUNK_ID) == 0)
			//		AddEntry(n1.offset, ".wav");

			//	i++;
			//}

			//for (auto i : entries)
			//{
			//	std::string f = i.first + ": " + std::to_string(i.second) + "\n";
			//	fwrite(f.c_str(), f.length(), 1, file);
			//}

			//fwrite("\n", 1, 1, file);

			//std::string text = getPrintText(root.chunk_id, root.offset, root.ChunkSize(), false);
			//fwrite(text.c_str(), text.length(), 1, file);
			//fwrite(full_text.c_str(), full_text.length(), 1, file);

			//fclose(file);
			//printf("%f", 0.0f);
		}
	}
}