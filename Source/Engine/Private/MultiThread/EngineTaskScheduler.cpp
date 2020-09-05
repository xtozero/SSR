#include "stdafx.h"
#include "MultiThread/EngineTaskScheduler.h"

#include "Core/InterfaceFactories.h"

#include <cstddef>

bool IsInGameThread( )
{
	return GetInterface<ITaskScheduler>( )->GetThisThreadTyep( ) == ThreadType::GameThread;
}

class EngineTaskScheduler : public ITaskScheduler
{
public:
	virtual [[nodiscard]] GroupHandle GetTaskGroup( std::size_t reserveSize = 0, std::size_t workerAffinity = std::numeric_limits<std::size_t>::max( ) ) override;

	virtual bool Run( GroupHandle handle, TaskBase* task ) override;

	virtual bool Wait( GroupHandle handle ) override;
	virtual void WaitAll( ) override;

	virtual void ProcessThisThreadTask( ) override;

	virtual bool IsComplete( GroupHandle handle ) const override;

	virtual std::size_t GetThisThreadTyep( ) const override;

	EngineTaskScheduler( );
	~EngineTaskScheduler( ) = default;
	EngineTaskScheduler( const EngineTaskScheduler& ) = delete;
	EngineTaskScheduler& operator=( const EngineTaskScheduler& ) = delete;
	EngineTaskScheduler( EngineTaskScheduler&& ) = delete;
	EngineTaskScheduler& operator=( EngineTaskScheduler&& ) = delete;

private:
	static constexpr std::size_t MAX_ENGINE_THREAD_GROUP = 1024;

	TaskScheduler m_taskScheduler;
};

GroupHandle EngineTaskScheduler::GetTaskGroup( std::size_t reserveSize, std::size_t workerAffinity )
{
	return m_taskScheduler.GetTaskGroup( reserveSize, workerAffinity );
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

std::size_t EngineTaskScheduler::GetThisThreadTyep( ) const
{
	return m_taskScheduler.GetThisThreadType( );
}

EngineTaskScheduler::EngineTaskScheduler( ) : m_taskScheduler( MAX_ENGINE_THREAD_GROUP, ThreadType::WorkerThreadCount )
{
}

ITaskScheduler* CreateTaskScheduler( )
{
	return new EngineTaskScheduler();
}

void DestroyTaskScheduler( ITaskScheduler* taskScheduler )
{
	delete taskScheduler;
}
