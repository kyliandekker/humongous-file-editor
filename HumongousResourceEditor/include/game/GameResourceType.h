#pragma once

#include <string>

namespace resource_editor
{
	namespace game
	{
		enum GameResourceType
		{
			Unknown,
			Talkie,
			SFX,
			Song,
			RoomBackground,
			RoomImage,
			RoomImageLayer,
			Local_Script,
			Global_Script,
			Verb_Script,
			Room,
			Category
		};

		inline std::string getExtension(GameResourceType a_ResourceType)
		{
			switch (a_ResourceType)
			{
				case Unknown:
				{
					return "";
				}
				case Talkie:
				case SFX:
				case Song:
				{
					return ".wav";
				}
				case RoomBackground:
				case RoomImage:
				case RoomImageLayer:
				{
					return ".png";
				}
				case Local_Script:
				case Global_Script:
				case Verb_Script:
				{
					return ".txt";
				}
				case Room:
				{
					return "";
				}
			}
			return "";
		}

		inline std::string getResourceTypeName(GameResourceType a_ResourceType)
		{
			switch (a_ResourceType)
			{
				case Unknown:
				{
					return "UNKNOWN";
				}
				case Talkie:
				{
					return "Talkie";
				}
				case SFX:
				{
					return "SFX";
				}
				case Song:
				{
					return "Song";
				}
				case RoomBackground:
				{
					return "Room Background";
				}
				case RoomImage:
				{
					return "Room Image";
				}
				case RoomImageLayer:
				{
					return "Room Image Layer";
				}
				case Local_Script:
				{
					return "Local Script";
				}
				case Global_Script:
				{
					return "Global Script";
				}
				case Verb_Script:
				{
					return "Verb Script";
				}
				case Room:
				{
					return "Room";
				}
			}
			return "";
		}
	}
}