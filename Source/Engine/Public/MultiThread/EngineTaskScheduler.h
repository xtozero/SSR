#pragma once

#include "common.h"
#include "TaskScheduler.h"

#include <limits>

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

template <std::size_t... N>
constexpr std::size_t WorkerAffinityMask( )
{
	return ( ( 1 << N ) | ... );
}

class ITaskScheduler
{
public:
	virtual [[nodiscard]] GroupHandle GetTaskGroup( std::size_t reserveSize = 0, std::size_t workerAffinity = std::numeric_limits<std::size_t>::max( ) ) = 0;

	virtual bool Run( GroupHandle handle, TaskBase* task ) = 0;

	virtual bool Wait( GroupHandle handle ) = 0;
	virtual void WaitAll( ) = 0;

	virtual bool IsComplete( GroupHandle handle ) const = 0;

	virtual ~ITaskScheduler( ) = default;
};

ITaskScheduler* CreateTaskScheduler( );
void DestroyTaskScheduler( ITaskScheduler* taskScheduler );