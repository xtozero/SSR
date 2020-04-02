#pragma once

#include "TaskScheduler.h"

#include "common.h"

namespace ThreadType
{
	enum
	{
		GameThread = -1,
		FileSystemThread,
		WorkerThread0,
		WorkerThread1,
		WorkerThread2,
		WorkerThread3,
		Count
	};
}

class EngineTaskScheduler
{
public:
	ENGINE_DLL GroupHandle GetTaskGroup( std::size_t reserveSize = 0 );

	ENGINE_DLL bool Run( GroupHandle handle, TaskBase* task );

	ENGINE_DLL bool Wait( GroupHandle handle );
	ENGINE_DLL void WaitAll( );

	ENGINE_DLL bool IsComplete( GroupHandle handle ) const;

	EngineTaskScheduler( );
	~EngineTaskScheduler( ) = default;
	EngineTaskScheduler( const EngineTaskScheduler& ) = delete;
	EngineTaskScheduler& operator=( const EngineTaskScheduler& ) = delete;
	EngineTaskScheduler( EngineTaskScheduler&& ) = delete;
	EngineTaskScheduler& operator=( EngineTaskScheduler&& ) = delete;

private:
	static constexpr std::size_t MAX_ENGINE_THREAD_GROUP = 1024;

	TaskScheduler m_taskScheduler;
};

extern ENGINE_DLL EngineTaskScheduler* g_TaskScheduler;