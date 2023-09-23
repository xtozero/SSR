#pragma once

#include "ThinkFunction.h"

#include <set>
#include <vector>

namespace logic
{
	class World;

	class ThinkTaskManager
	{
	public:
		void RegisterThinkFunction( ThinkFunction* thinkFunction );
		void UnRegisterThinkFunction( ThinkFunction* thinkFunction );

		void BeginFrame( float totalTime );

		void RunThinkGroup( ThinkingGroup group, float elapsedTime );

	private:
		std::set<ThinkFunction*> m_thinkFunctions;

		std::vector<ThinkFunction*> m_thinkTasks[static_cast<uint32>( ThinkingGroup::Max )];
	};
}
