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
			RoomBackground_Palette,
			RoomImage,
			RoomImage_Layer,
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
				case RoomBackground_Palette:
				case RoomImage:
				case RoomImage_Layer:
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
	}
}