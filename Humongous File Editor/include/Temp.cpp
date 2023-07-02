#include "Temp.h"

#include "lowlevel/FileContainer.h"
#include <assert.h>

namespace HumongousFileEditor
{
	namespace chunk_reader
	{
		void Temp::Read(const char* path)
		{
			FileContainer fc = FileContainer(path);

			// START TEST (ROOT).
			Node n1 = fc.Start();
			assert(n1.ChunkSize() == 114231949);

			// CHUNK RIGHT AFTER ROOT TEST.
			Node n2 = n1.Child();
			assert(n2.ChunkSize() == 50554);

			// CHUNK AFTER THE N2.
			Node n3 = n2.Next();
			assert(n3.ChunkSize() == 63886);

			// CHUNK BEFORE N3 (WHICH SHOULD BE N2).
			Node n4 = n3.Previous();
			assert(n4.ChunkSize() == n2.ChunkSize());

			// GETTING CHUNK VIA OFFSET (WHICH SHOULD BE N2).
			Node n5 = fc.NodeFromOffset(n2.offset);
			assert(n5.ChunkSize() == n2.ChunkSize());

			// EOF.
			Node n6 = n1.Next();
			assert(n6.null);

			// BOF.
			Node n7 = n1.Previous();
			assert(n1.ChunkSize() == n7.ChunkSize());

			printf("%f", 0.0f);
		}
	}
}