#include "stdafx.h"
#include "MultiThread/EngineTaskScheduler.h"

#include "InterfaceFactories.h"

#include <cstddef>

class EngineTaskScheduler : public ITaskScheduler
{
public:
	[[nodiscard]] virtual TaskHandle GetTaskGroup( ) override;

	virtual bool Run( TaskHandle handle ) override;

	virtual bool Wait( TaskHandle handle ) override;

	virtual void ProcessThisThreadTask( ) override;

	virtual size_t GetThisThreadType( ) const override;

	[[nodiscard]] TaskHandle GetExclusiveTaskGroup( size_t threadType );

	EngineTaskScheduler( );
	~EngineTaskScheduler( ) = default;
	EngineTaskScheduler( const EngineTaskScheduler& ) = delete;
	EngineTaskScheduler& operator=( const EngineTaskScheduler& ) = delete;
	EngineTaskScheduler( EngineTaskScheduler&& ) = delete;
	EngineTaskScheduler& operator=( EngineTaskScheduler&& ) = delete;

private:
	static constexpr size_t MAX_ENGINE_THREAD_GROUP = 1024;

	TaskScheduler m_taskScheduler;
};

TaskHandle EngineTaskScheduler::GetTaskGroup( )
{
	return m_taskScheduler.GetTaskGroup( );
}

bool EngineTaskScheduler::Run( TaskHandle handle )
{
	return m_taskScheduler.Run( handle );
}

bool EngineTaskScheduler::Wait( TaskHandle handle )
{
	return m_taskScheduler.Wait( handle );
}

void EngineTaskScheduler::ProcessThisThreadTask( )
{
	m_taskScheduler.ProcessThisThreadTask( );
}

size_t EngineTaskScheduler::GetThisThreadType( ) const
{
	return m_taskScheduler.GetThisThreadType( );
}

TaskHandle EngineTaskScheduler::GetExclusiveTaskGroup( size_t threadType )
{
	return m_taskScheduler.GetExclusiveTaskGroup( threadType );
}

EngineTaskScheduler::EngineTaskScheduler( ) : m_taskScheduler( MAX_ENGINE_THREAD_GROUP, ThreadType::WorkerThreadCount )
{ }

ITaskScheduler* CreateTaskScheduler( )
{
	return new EngineTaskScheduler();
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
	auto taskScheduler = static_cast<EngineTaskScheduler*>( GetInterface<ITaskScheduler>( ) );
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
