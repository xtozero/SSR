#include "stdafx.h"
#include "MultiThread/EngineTaskScheduler.h"

#include "Core/InterfaceFactories.h"

#include <cstddef>

class EngineTaskScheduler : public ITaskScheduler
{
public:
	[[nodiscard]] virtual GroupHandle GetTaskGroup( std::size_t workerAffinity = std::numeric_limits<std::size_t>::max( ) ) override;

	virtual bool Run( GroupHandle handle, TaskBase* task ) override;

	virtual bool Wait( GroupHandle handle ) override;
	virtual void WaitAll( ) override;

	virtual void ProcessThisThreadTask( ) override;

	virtual bool IsComplete( GroupHandle handle ) const override;

	virtual std::size_t GetThisThreadType( ) const override;

	[[nodiscard]] GroupHandle GetOrderedTaskGroup( int threadType );

	EngineTaskScheduler( );
	~EngineTaskScheduler( ) = default;
	EngineTaskScheduler( const EngineTaskScheduler& ) = delete;
	EngineTaskScheduler& operator=( const EngineTaskScheduler& ) = delete;
	EngineTaskScheduler( EngineTaskScheduler&& ) = delete;
	EngineTaskScheduler& operator=( EngineTaskScheduler&& ) = delete;

private:
	static constexpr std::size_t MAX_ENGINE_THREAD_GROUP = 1024;

	TaskScheduler m_taskScheduler;
	GroupHandle m_orderedTask[ThreadType::WorkerThreadCount];
};

GroupHandle EngineTaskScheduler::GetTaskGroup( std::size_t workerAffinity )
{
	return m_taskScheduler.GetTaskGroup( workerAffinity );
}

bool EngineTaskScheduler::Run( GroupHandle handle, TaskBase* task )
{
	return m_taskScheduler.Run( handle, task );
}

bool EngineTaskScheduler::Wait( GroupHandle handle )
{
	return m_taskScheduler.Wait( handle );
}

void EngineTaskScheduler::WaitAll( )
{
	m_taskScheduler.WaitAll( );
}

void EngineTaskScheduler::ProcessThisThreadTask( )
{
	m_taskScheduler.ProcessThisThreadTask( );
}

bool EngineTaskScheduler::IsComplete( GroupHandle handle ) const
{
	return m_taskScheduler.IsComplete( handle );
}

std::size_t EngineTaskScheduler::GetThisThreadType( ) const
{
	return m_taskScheduler.GetThisThreadType( );
}

GroupHandle EngineTaskScheduler::GetOrderedTaskGroup( int threadType )
{
	return m_orderedTask[threadType];
}

EngineTaskScheduler::EngineTaskScheduler( ) : m_taskScheduler( MAX_ENGINE_THREAD_GROUP, ThreadType::WorkerThreadCount )
{
	for ( std::size_t threadType = 0; threadType < ThreadType::WorkerThreadCount; ++threadType )
	{
		m_orderedTask[threadType] = GetTaskGroup( 1i64 << threadType );
	}
}

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
	auto taskScheduler = static_cast<EngineTaskScheduler*>( GetInterface<ITaskScheduler>( ) );
	GroupHandle taskGroup = taskScheduler->GetOrderedTaskGroup( ThreadType::RenderThread );
	bool success = taskScheduler->Run( taskGroup, task );
	assert( success );
}