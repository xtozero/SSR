#pragma once

#include "common.h"
#include "EnumClassFlags.h"
#include "InterfaceFactories.h"
#include "SizedTypes.h"
#include "TaskSchedulerCore.h"

#include <limits>

enum class ThreadType : uint8
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
ENUM_CLASS_FLAGS( ThreadType );

class ITaskScheduler
{
public:
	[[nodiscard]] virtual TaskHandle GetTaskGroup() = 0;

	virtual bool Run( TaskHandle handle ) = 0;

	virtual bool Wait( TaskHandle handle ) = 0;

	virtual void ProcessThisThreadTask() = 0;

	virtual size_t GetThisThreadType() const = 0;

	virtual ~ITaskScheduler() = default;
};

#define WorkerThreads ThreadType::WorkerThread0, ThreadType::WorkerThread1, ThreadType::WorkerThread2, ThreadType::WorkerThread3

template <ThreadType... N>
constexpr size_t WorkerAffinityMask()
{
	return ( ( 1 << static_cast<uint8>( N ) ) | ... );
}

ITaskScheduler* CreateTaskScheduler();
void DestroyTaskScheduler( ITaskScheduler* taskScheduler );

bool IsInGameThread();
bool IsInRenderThread();
void EnqueueRenderTask( TaskBase* task );

template <typename Lambda>
class LambdaTask
{
public:
	void DoTask()
	{
		m_lambda();
	}

	LambdaTask( const Lambda& lambda ) : m_lambda( lambda ) {}

private:
	Lambda m_lambda;
};

template <ThreadType... N, typename Lambda>
TaskHandle EnqueueThreadTask( Lambda lambda )
{
	ITaskScheduler* taskScheduler = GetInterface<ITaskScheduler>();
	constexpr size_t affinityMask = WorkerAffinityMask<N...>();
	TaskHandle taskGroup = taskScheduler->GetTaskGroup();
	taskGroup.AddTask( Task<LambdaTask<Lambda>>::Create( affinityMask, lambda ) );
	[[maybe_unused]] bool success = taskScheduler->Run( taskGroup );
	assert( success );
	return taskGroup;
}

template <typename Lambda>
void EnqueueRenderTask( Lambda lambda )
{
	if ( IsInRenderThread() )
	{
		lambda();
	}
	else
	{
		auto* task = Task<LambdaTask<Lambda>>::Create( WorkerAffinityMask<ThreadType::RenderThread>(), lambda );
		EnqueueRenderTask( static_cast<TaskBase*>( task ) );
	}
}