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
GroupHandle ENQUEUE_THREAD_TASK( Lambda lambda )
{
	ITaskScheduler* taskScheduler = GetInterface<ITaskScheduler>( );
	constexpr std::size_t afinityMask = WorkerAffinityMask<N...>( );
	GroupHandle taskGroup = taskScheduler->GetTaskGroup( 1, afinityMask );
	bool success = taskScheduler->Run( taskGroup, Task<LambdaTask<Lambda>>::Create( TASK_TYPE::FIRE_AND_FORGET, lambda ) );
	assert( success );
	return taskGroup;
}

ENGINE_FUNC_DLL bool IsInGameThread( );
ENGINE_FUNC_DLL bool IsInRenderThread( );

class ITaskScheduler
{
public:
	virtual [[nodiscard]] GroupHandle GetTaskGroup( std::size_t reserveSize = 0, std::size_t workerAffinity = std::numeric_limits<std::size_t>::max( ) ) = 0;

	virtual bool Run( GroupHandle handle, TaskBase* task ) = 0;

	virtual bool Wait( GroupHandle handle ) = 0;
	virtual void WaitAll( ) = 0;

	virtual void ProcessThisThreadTask( ) = 0;

	virtual bool IsComplete( GroupHandle handle ) const = 0;

	virtual std::size_t GetThisThreadTyep( ) const = 0;

	virtual ~ITaskScheduler( ) = default;
};

ITaskScheduler* CreateTaskScheduler( );
void DestroyTaskScheduler( ITaskScheduler* taskScheduler );
