#include "file/Files.h"

#include "lowlevel/HumongousChunkDefinitions.h"
#include "lowlevel/FileContainer.h"
#include "lowlevel/utils.h"

HumongousFileEditor::files::Files HumongousFileEditor::files::FILES;

namespace HumongousFileEditor
{
	namespace files
	{
		chunk_reader::FileContainer* Files::Read(const char* path)
		{
			// Check if we recognize the extension. If not, do not load it at all.
			files::FileType fileType = files::getFileTypeByExtension(path);
			if (fileType == files::FileType_Unknown)
				return nullptr;

			// Load the file.
			chunk_reader::FileContainer* fc = new chunk_reader::FileContainer(path);
			fc->fileType = fileType;
			fc->path = path;

			// Load the first header.
			chunk_reader::ChunkInfo header = fc->GetChunkInfo(0);

			switch (fileType)
			{
				case files::FileType_A:
				{
					if (utils::chunkcmp(header.chunk_id, chunk_reader::LECF_CHUNK_ID) != 0)
					{
						// If the first check fails, then decrypt. Maybe we already have a decrypted file, in which case it'd succeed.
						fc->Decrypt(0x69);

						// If after decryption it still does not start with a (a) chunk, return and give an error.
						header = fc->GetChunkInfo(0);
						if (utils::chunkcmp(header.chunk_id, chunk_reader::LECF_CHUNK_ID) != 0)
							return nullptr;
					}

					if (a != nullptr)
						delete a;
					a = fc;
					break;
				}
				case files::FileType_HE2:
				{
					// File does not start with HE2 chunk.
					if (utils::chunkcmp(header.chunk_id, chunk_reader::TLKB_CHUNK_ID) != 0)
						return nullptr;

					if (he2 != nullptr)
						delete he2;
					he2 = fc;
					break;
				}
				case files::FileType_HE4:
				{
					// File does not start with HE4 chunk.
					if (utils::chunkcmp(header.chunk_id, chunk_reader::SONG_CHUNK_ID) != 0)
						return nullptr;

					if (he4 != nullptr)
						delete he4;
					he4 = fc;
					break;
				}
				case files::FileType_HE0:
				{
					if (utils::chunkcmp(header.chunk_id, chunk_reader::MAXS_CHUNK_ID) != 0)
					{
						// If the first check fails, then decrypt. Maybe we already have a decrypted file, in which case it'd succeed.
						fc->Decrypt(0x69);

						// If after decryption it still does not start with a HE0 chunk, return and give an error.
						header = fc->GetChunkInfo(0);
						if (utils::chunkcmp(header.chunk_id, chunk_reader::MAXS_CHUNK_ID) != 0)
							return nullptr;
					}

					if (he0 != nullptr)
						delete he0;
					he0 = fc;
					break;
				}
				default:
				{
					return nullptr;
				}
			}

			return fc;
		}
        chunk_reader::FileContainer* Files::getFile(FileType fileType)
        {
			switch (fileType)
			{
				case files::FileType_A:
					return a;
				case files::FileType_HE2:
					return he2;
				case files::FileType_HE4:
					return he4;
				case files::FileType_HE0:
					return he0;
			}
			return nullptr;
        }
	}
}