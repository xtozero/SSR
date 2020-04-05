#include "stdafx.h"
#include "MultiThread/EngineTaskScheduler.h"

#include <cstddef>

class EngineTaskScheduler : public ITaskScheduler
{
public:
	virtual [[nodiscard]] GroupHandle GetTaskGroup( std::size_t reserveSize = 0 ) override;

	virtual bool Run( GroupHandle handle, TaskBase* task ) override;

	virtual bool Wait( GroupHandle handle ) override;
	virtual void WaitAll( ) override;

	virtual bool IsComplete( GroupHandle handle ) const override;

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

GroupHandle EngineTaskScheduler::GetTaskGroup( std::size_t reserveSize )
{
	return m_taskScheduler.GetTaskGroup( reserveSize );
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

bool EngineTaskScheduler::IsComplete( GroupHandle handle ) const
{
	return m_taskScheduler.IsComplete( handle );
}

EngineTaskScheduler::EngineTaskScheduler( ) : m_taskScheduler( MAX_ENGINE_THREAD_GROUP, ThreadType::Count )
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
