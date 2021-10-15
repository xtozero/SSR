#include "TaskScheduler.h"

#include "InterfaceFactories.h"

#include <cstddef>

class TaskSchedulerImpl : public ITaskScheduler
{
public:
	[[nodiscard]] virtual TaskHandle GetTaskGroup( ) override;

	virtual bool Run( TaskHandle handle ) override;

	virtual bool Wait( TaskHandle handle ) override;

	virtual void ProcessThisThreadTask( ) override;

	virtual size_t GetThisThreadType( ) const override;

	[[nodiscard]] TaskHandle GetExclusiveTaskGroup( size_t threadType );

	TaskSchedulerImpl( );
	~TaskSchedulerImpl( ) = default;
	TaskSchedulerImpl( const TaskSchedulerImpl& ) = delete;
	TaskSchedulerImpl& operator=( const TaskSchedulerImpl& ) = delete;
	TaskSchedulerImpl( TaskSchedulerImpl&& ) = delete;
	TaskSchedulerImpl& operator=( TaskSchedulerImpl&& ) = delete;

private:
	static constexpr size_t MAX_ENGINE_THREAD_GROUP = 1024;

	TaskScheduler m_taskScheduler;
};

TaskHandle TaskSchedulerImpl::GetTaskGroup( )
{
	return m_taskScheduler.GetTaskGroup( );
}

bool TaskSchedulerImpl::Run( TaskHandle handle )
{
	return m_taskScheduler.Run( handle );
}

bool TaskSchedulerImpl::Wait( TaskHandle handle )
{
	return m_taskScheduler.Wait( handle );
}

void TaskSchedulerImpl::ProcessThisThreadTask( )
{
	m_taskScheduler.ProcessThisThreadTask( );
}

size_t TaskSchedulerImpl::GetThisThreadType( ) const
{
	return m_taskScheduler.GetThisThreadType( );
}

TaskHandle TaskSchedulerImpl::GetExclusiveTaskGroup( size_t threadType )
{
	return m_taskScheduler.GetExclusiveTaskGroup( threadType );
}

TaskSchedulerImpl::TaskSchedulerImpl( ) : m_taskScheduler( MAX_ENGINE_THREAD_GROUP, ThreadType::WorkerThreadCount )
{ }

ITaskScheduler* CreateTaskScheduler( )
{
	return new TaskSchedulerImpl();
}

void DestroyTaskScheduler( ITaskScheduler* taskScheduler )
{
	delete taskScheduler;
}

bool IsInGameThread( )
{
	if ( GetInterface<ITaskScheduler>( ) )
	{
		return GetInterface<ITaskScheduler>( )->GetThisThreadType( ) == ThreadType::GameThread;
	}

	return true;
}

bool IsInRenderThread( )
{
	return GetInterface<ITaskScheduler>( )->GetThisThreadType( ) == ThreadType::RenderThread;
}

void EnqueueRenderTask( TaskBase* task )
{
	assert( task->WorkerAffinity( ) == WorkerAffinityMask<ThreadType::RenderThread>( ) );
	auto taskScheduler = static_cast<TaskSchedulerImpl*>( GetInterface<ITaskScheduler>( ) );
	TaskHandle taskGroup = taskScheduler->GetExclusiveTaskGroup( ThreadType::RenderThread );
	taskGroup.AddTask( task );
	[[maybe_unused]] bool success = taskScheduler->Run( taskGroup );
	assert( success );
}

void WaitRenderThread( )
{
	TaskHandle handle = EnqueueThreadTask<ThreadType::RenderThread>( []( ) {} );
	GetInterface<ITaskScheduler>( )->Wait( handle );
}
