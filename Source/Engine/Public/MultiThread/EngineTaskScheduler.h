#include "TaskScheduler.h"

#include "common.h"

class EngineTaskScheduler
{
public:
	ENGINE_DLL GroupHandle GetTaskGroup( std::size_t reserveSize = 0 );

	ENGINE_DLL bool Run( GroupHandle handle, WorkerFunc func, void* context );

	ENGINE_DLL bool Wait( GroupHandle handle );
	ENGINE_DLL void WaitAll( );

	ENGINE_DLL bool IsComplete( GroupHandle handle ) const;

	EngineTaskScheduler( );

private:
	static constexpr std::size_t MAX_ENGINE_THREAD_GROUP = 1024;

	TaskScheduler m_taskScheduler;
};

extern ENGINE_DLL EngineTaskScheduler* g_TaskScheduler;