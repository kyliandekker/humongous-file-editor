#include "game/decompiled_resource/ScriptResource.h"

namespace resource_editor
{
	namespace game
	{
		ScriptResource::ScriptResource(game::GameResource& a_Resource) : DecompiledResource(a_Resource)
		{
			GetData(a_Resource);
		}

		ScriptResource::ScriptResource(const ScriptResource& rhs) : DecompiledResource(rhs)
		{
		}

		ScriptResource::~ScriptResource()
		{
		}

		bool ScriptResource::GetData(game::GameResource& a_Resource)
		{
			return true;
		}
	}
}