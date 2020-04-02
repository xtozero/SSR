#include "stdafx.h"
#include "MultiThread/EngineTaskScheduler.h"

#include <cstddef>

EngineTaskScheduler* g_TaskScheduler = nullptr;

GroupHandle EngineTaskScheduler::GetTaskGroup( std::size_t reserveSize )
{
	return m_taskScheduler.GetTaskGroup( reserveSize );
}

bool EngineTaskScheduler::Run( GroupHandle handle, TaskBase* task )
{
	return m_taskScheduler.Run( handle, task );
}

bool EngineTaskScheduler::Wait( GroupHandle handle )
{
	return m_taskScheduler.Wait( handle );
}

void EngineTaskScheduler::WaitAll( )
{
	m_taskScheduler.WaitAll( );
}

bool EngineTaskScheduler::IsComplete( GroupHandle handle ) const
{
	return m_taskScheduler.IsComplete( handle );
}

EngineTaskScheduler::EngineTaskScheduler( ) : m_taskScheduler( MAX_ENGINE_THREAD_GROUP, ThreadType::Count )
{
}
