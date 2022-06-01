#include "Core/ThinkTaskManager.h"

#include <cassert>

void ThinkTaskManager::RegisterThinkFunction( ThinkFunction* thinkFunction )
{
	m_thinkFunctions.emplace( thinkFunction );
}

void ThinkTaskManager::UnRegisterThinkFunction( ThinkFunction* thinkFunction )
{
	m_thinkFunctions.erase( thinkFunction );
}

void ThinkTaskManager::BeginFrame( float totalTime )
{
	for ( std::vector<ThinkFunction*>& thinkTask : m_thinkTasks )
	{
		thinkTask.clear();
		thinkTask.reserve( m_thinkFunctions.size() );
	}

	for ( ThinkFunction* thinkFunction : m_thinkFunctions )
	{
		auto& internalData = *thinkFunction->m_internalData;
		if ( ( totalTime - internalData.m_lastThinkTime ) > thinkFunction->m_thinkInterval )
		{
			internalData.m_lastThinkTime = totalTime;
		}
		else
		{
			continue;
		}

		ThinkingGroup group = thinkFunction->m_thinkGroup;
		m_thinkTasks[static_cast<uint32>( group )].push_back( thinkFunction );
	}
}

void ThinkTaskManager::RunThinkGroup( ThinkingGroup group, float elapsedTime )
{
	assert( ThinkingGroup::PrePhysics <= group && group < ThinkingGroup::Max );

	auto nGroup = static_cast<size_t>( group );
	for ( ThinkFunction* thinkTask : m_thinkTasks[nGroup] )
	{
		thinkTask->ExecuteThink( elapsedTime );
	}
}
