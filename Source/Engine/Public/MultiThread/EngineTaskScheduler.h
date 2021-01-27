#pragma once

#include "common.h"
#include "Core/InterfaceFactories.h"
#include "TaskScheduler.h"

#include <limits>

namespace ThreadType
{
	enum
	{
		FileSystemThread,
		WorkerThread0,
		WorkerThread1,
		WorkerThread2,
		WorkerThread3,
		// if add new thread type, insert here
		RenderThread,
		GameThread,
		WorkerThreadCount = GameThread,
	};
}

template <std::size_t... N>
constexpr std::size_t WorkerAffinityMask( )
{
	return ( ( 1 << N ) | ... );
}

template <typename Lambda>
class LambdaTask
{
public:
	void DoTask( )
	{
		m_lambda( );
	}

	LambdaTask( const Lambda& lambda ) : m_lambda( lambda ) {}

private:
	Lambda m_lambda;
};

template <std::size_t... N, typename Lambda>
GroupHandle EnqueueThreadTask( Lambda lambda, TASK_TYPE taskType = TASK_TYPE::WAITABLE )
{
	ITaskScheduler* taskScheduler = GetInterface<ITaskScheduler>( );
	constexpr std::size_t afinityMask = WorkerAffinityMask<N...>( );
	GroupHandle taskGroup = taskScheduler->GetTaskGroup( afinityMask );
	bool success = taskScheduler->Run( taskGroup, Task<LambdaTask<Lambda>>::Create( taskType, lambda ) );
	assert( success );
	return taskGroup;
}

template <typename Lambda>
void EnqueueRenderTask( Lambda lambda )
{
	auto* task = Task<LambdaTask<Lambda>>::Create( TASK_TYPE::WAITABLE, lambda );
	EnqueueRenderTask( static_cast<TaskBase*>( task ) );
}

class ITaskScheduler
{
public:
	[[nodiscard]] virtual GroupHandle GetTaskGroup( std::size_t workerAffinity = std::numeric_limits<std::size_t>::max( ) ) = 0;

	virtual bool Run( GroupHandle handle, TaskBase* task ) = 0;

	virtual bool Wait( GroupHandle handle ) = 0;
	virtual void WaitAll( ) = 0;

	virtual void ProcessThisThreadTask( ) = 0;

	virtual bool IsComplete( GroupHandle handle ) const = 0;

	virtual std::size_t GetThisThreadType( ) const = 0;

	virtual ~ITaskScheduler( ) = default;
};

ITaskScheduler* CreateTaskScheduler( );
void DestroyTaskScheduler( ITaskScheduler* taskScheduler );

ENGINE_FUNC_DLL bool IsInGameThread( );
ENGINE_FUNC_DLL bool IsInRenderThread( );
ENGINE_FUNC_DLL void EnqueueRenderTask( TaskBase* task );
