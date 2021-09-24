#pragma once

#include "common.h"
#include "Core/InterfaceFactories.h"
#include "SizedTypes.h"
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

#define WorkerThreads ThreadType::WorkerThread0, ThreadType::WorkerThread1, ThreadType::WorkerThread2, ThreadType::WorkerThread3

template <size_t... N>
constexpr size_t WorkerAffinityMask( )
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

template <size_t... N, typename Lambda>
TaskHandle EnqueueThreadTask( Lambda lambda )
{
	ITaskScheduler* taskScheduler = GetInterface<ITaskScheduler>( );
	constexpr size_t affinityMask = WorkerAffinityMask<N...>( );
	TaskHandle taskGroup = taskScheduler->GetTaskGroup( );
	taskGroup.AddTask( Task<LambdaTask<Lambda>>::Create( affinityMask, lambda ) );
	[[maybe_unused]] bool success = taskScheduler->Run( taskGroup );
	assert( success );
	return taskGroup;
}

template <typename Lambda>
void EnqueueRenderTask( Lambda lambda )
{
	if ( IsInRenderThread( ) )
	{
		lambda( );
	}
	else
	{
		auto* task = Task<LambdaTask<Lambda>>::Create( WorkerAffinityMask<ThreadType::RenderThread>( ), lambda );
		EnqueueRenderTask( static_cast<TaskBase*>( task ) );
	}
}

class ITaskScheduler
{
public:
	[[nodiscard]] virtual TaskHandle GetTaskGroup( ) = 0;

	virtual bool Run( TaskHandle handle ) = 0;

	virtual bool Wait( TaskHandle handle ) = 0;

	virtual void ProcessThisThreadTask( ) = 0;

	virtual size_t GetThisThreadType( ) const = 0;

	virtual ~ITaskScheduler( ) = default;
};

ITaskScheduler* CreateTaskScheduler( );
void DestroyTaskScheduler( ITaskScheduler* taskScheduler );

ENGINE_FUNC_DLL bool IsInGameThread( );
ENGINE_FUNC_DLL bool IsInRenderThread( );
ENGINE_FUNC_DLL void EnqueueRenderTask( TaskBase* task );
