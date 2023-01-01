#include "TaskScheduler.h"

#include "InterfaceFactories.h"

#include <cstddef>

namespace
{
	const char* WorkerNames[] = {
		"FileSystemThread",
		"WorkerThread0",
		"WorkerThread1",
		"WorkerThread2",
		"WorkerThread3",
		// if add new thread type, insert here
		"RenderThread",
		"GameThread",
	};
}

class TaskSchedulerImpl : public ITaskScheduler
{
public:
	[[nodiscard]] virtual TaskHandle GetTaskGroup() override;

	virtual bool Run( TaskHandle handle ) override;

	virtual bool Wait( TaskHandle handle ) override;

	virtual void ProcessThisThreadTask() override;

	virtual size_t GetThisThreadType() const override;

	[[nodiscard]] TaskHandle GetExclusiveTaskGroup( ThreadType threadType );

	TaskSchedulerImpl();
	virtual ~TaskSchedulerImpl() override = default;
	TaskSchedulerImpl( const TaskSchedulerImpl& ) = delete;
	TaskSchedulerImpl& operator=( const TaskSchedulerImpl& ) = delete;
	TaskSchedulerImpl( TaskSchedulerImpl&& ) = delete;
	TaskSchedulerImpl& operator=( TaskSchedulerImpl&& ) = delete;

private:
	static constexpr size_t MAX_ENGINE_THREAD_GROUP = 1024;

	TaskScheduler m_taskScheduler;
};

TaskHandle TaskSchedulerImpl::GetTaskGroup()
{
	return m_taskScheduler.GetTaskGroup();
}

bool TaskSchedulerImpl::Run( TaskHandle handle )
{
	return m_taskScheduler.Run( handle );
}

bool TaskSchedulerImpl::Wait( TaskHandle handle )
{
	return m_taskScheduler.Wait( handle );
}

void TaskSchedulerImpl::ProcessThisThreadTask()
{
	m_taskScheduler.ProcessThisThreadTask();
}

size_t TaskSchedulerImpl::GetThisThreadType() const
{
	return m_taskScheduler.GetThisThreadType();
}

TaskHandle TaskSchedulerImpl::GetExclusiveTaskGroup( ThreadType threadType )
{
	return m_taskScheduler.GetExclusiveTaskGroup( static_cast<size_t>( threadType ) );
}

TaskSchedulerImpl::TaskSchedulerImpl() : m_taskScheduler( MAX_ENGINE_THREAD_GROUP, static_cast<size_t>( ThreadType::WorkerThreadCount ) )
{ 
	static_assert( ( static_cast<uint32>( ThreadType::WorkerThreadCount ) + 1 ) == std::extent_v<decltype( WorkerNames )> );
	for ( uint32 i = 0; i < static_cast<uint32>( ThreadType::WorkerThreadCount ); ++i )
	{
		m_taskScheduler.SetWorkerNameForDebugging( i, WorkerNames[i] );
	}
}

ITaskScheduler* CreateTaskScheduler()
{
	return new TaskSchedulerImpl();
}

void DestroyTaskScheduler( ITaskScheduler* taskScheduler )
{
	delete taskScheduler;
}

bool IsInGameThread()
{
	if ( GetInterface<ITaskScheduler>() )
	{
		return static_cast<ThreadType>( GetInterface<ITaskScheduler>()->GetThisThreadType() ) == ThreadType::GameThread;
	}

	return true;
}

bool IsInRenderThread()
{
	return static_cast<ThreadType>( GetInterface<ITaskScheduler>()->GetThisThreadType() ) == ThreadType::RenderThread;
}

void EnqueueRenderTask( TaskBase* task )
{
	assert( task->WorkerAffinity() == WorkerAffinityMask<ThreadType::RenderThread>() );
	auto taskScheduler = static_cast<TaskSchedulerImpl*>( GetInterface<ITaskScheduler>() );
	TaskHandle taskGroup = taskScheduler->GetExclusiveTaskGroup( ThreadType::RenderThread );
	taskGroup.AddTask( task );
	[[maybe_unused]] bool success = taskScheduler->Run( taskGroup );
	assert( success );
}
