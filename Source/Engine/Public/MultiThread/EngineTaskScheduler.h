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

class ITaskScheduler
{
public:
	virtual [[nodiscard]] GroupHandle GetTaskGroup( std::size_t reserveSize = 0 ) = 0;

	virtual bool Run( GroupHandle handle, TaskBase* task ) = 0;

	virtual bool Wait( GroupHandle handle ) = 0;
	virtual void WaitAll( ) = 0;

	virtual bool IsComplete( GroupHandle handle ) const = 0;

	virtual ~ITaskScheduler( ) = default;
};

ITaskScheduler* CreateTaskScheduler( );
void DestroyTaskScheduler( ITaskScheduler* taskScheduler );