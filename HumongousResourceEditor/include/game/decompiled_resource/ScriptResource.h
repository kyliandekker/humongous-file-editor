#pragma once

#include "./DecompiledResource.h"

#include "cmd/ScriptInstruction.h"

namespace resource_editor
{
	namespace game
	{
		class GameResource;

		class ScriptResource : public DecompiledResource
		{
		public:
			ScriptResource(game::GameResource& a_Resource);
			ScriptResource() = default;
			ScriptResource(const ScriptResource& rhs);
			~ScriptResource();
			bool GetData(game::GameResource& a_Resource);
			bool ReplaceResource(game::GameResource& a_Resource) override;

			std::vector<ScriptInstruction> m_Instructions;
		};
	}
}