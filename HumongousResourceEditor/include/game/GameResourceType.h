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
			Local_Script,
			Global_Script,
			Verb_Script,
			Room,
			Category
		};

		inline std::string getExtension(GameResourceType resourceType)
		{
			switch (resourceType)
			{
				case Unknown:
				{
					return "";
				}
				case Talkie:
				{
					return ".wav";
				}
				case SFX:
				{
					return ".wav";
				}
				case Song:
				{
					return ".wav";
				}
				case RoomBackground:
				{
					return ".png";
				}
				case RoomImage:
				{
					return ".png";
				}
				case Local_Script:
				{
					return ".txt";
				}
				case Global_Script:
				{
					return ".txt";
				}
				case Verb_Script:
				{
					return ".txt";
				}
				case Room:
				{
					return "";
				}
			}
		}
	}
}