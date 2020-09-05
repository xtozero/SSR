#include "TaskScheduler.h"

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <shared_mutex>
#include <vector>

namespace
{
	bool CheckWorkerAffinity( std::size_t workerId, std::size_t workerAffinity )
	{
		assert( std::numeric_limits<std::size_t>::digits > workerId );
		return ( ( 1i64 << workerId ) & workerAffinity ) > 0;
	}
}

struct TaskGroup
{
	std::vector<TaskBase*> m_tasks;
	std::shared_mutex m_taskLock;
	std::atomic<bool> m_free;
	std::atomic<std::size_t> m_reference;
	std::atomic<std::size_t> m_lastId;
	std::atomic<std::size_t> m_head;
	std::size_t m_workerAffinity;
};

struct Worker
{
	std::size_t m_threadType;
	std::thread m_thread;
	std::mutex m_lock;
	std::condition_variable m_cv;
	std::atomic<bool> m_wakeup;
};

void WorkerThread( TaskScheduler* scheduler, Worker* worker )
{
	scheduler->m_workerid[worker->m_threadType] = std::this_thread::get_id( );

	while ( true )
	{
		{
			std::unique_lock<std::mutex> lock( worker->m_lock );
			worker->m_cv.wait( lock, [&worker]( ) { return worker->m_wakeup.load( ); } );
		}
		
		worker->m_wakeup = false;

		while ( true )
		{
			if ( scheduler->m_shutdown )
			{
				return;
			}

			TaskGroup* group = nullptr;
			TaskBase* task = nullptr;
			for ( std::size_t i = 0
				; ( i < scheduler->m_maxTaskGroup ) && ( task == nullptr )
				; ++i )
			{
				group = &scheduler->m_taskGroups[i];
				if ( group->m_free || group->m_reference == 0 || ( CheckWorkerAffinity( worker->m_threadType, group->m_workerAffinity ) == false ) )
				{
					continue;
				}

				while ( task == nullptr )
				{
					std::size_t head = group->m_head.load( );
					if ( head >= group->m_tasks.size( ) )
					{
						break;
					}

					if ( group->m_head.compare_exchange_strong( head, head + 1 ) )
					{
						std::shared_lock taskLock( group->m_taskLock );
						task = group->m_tasks[head];
					}
				}
			}

			if ( task == nullptr )
			{
				break;
			}

			task->Execute( );
			--group->m_reference;
		}
	}
}

GroupHandle TaskScheduler::GetTaskGroup( std::size_t reserveSize, std::size_t workerAffinity )
{
	for ( std::size_t i = 0; i < m_maxTaskGroup; ++i )
	{
		TaskGroup& group = m_taskGroups[i];
		bool expected = true;
		if ( group.m_free.compare_exchange_strong( expected, false ) )
		{
			group.m_head = 0;
			std::unique_lock taskLock( group.m_taskLock );
			group.m_tasks.clear( );
			group.m_tasks.reserve( reserveSize );
			group.m_workerAffinity = workerAffinity;
			return GroupHandle{ i, ++group.m_lastId };
		}
	}

	return GroupHandle{ std::numeric_limits<std::size_t>::max( ), 0 };
}

bool TaskScheduler::Run( GroupHandle handle, TaskBase* task )
{
	if ( handle.m_groupIndex >= m_maxTaskGroup )
	{
		return false;
	}

	TaskGroup& group = m_taskGroups[handle.m_groupIndex];

	if ( group.m_free.load( ) )
	{
		return false;
	}

	if ( handle.m_id != group.m_lastId )
	{
		return false;
	}

	{
		std::unique_lock taskLock( group.m_taskLock );
		group.m_tasks.push_back( task );
	}
	
	++group.m_reference;

	for ( std::size_t i = 0; i < m_workerCount; ++i )
	{
		if ( CheckWorkerAffinity( i, group.m_workerAffinity ) )
		{
			std::unique_lock<std::mutex> lock( m_workers[i].m_lock );
			m_workers[i].m_wakeup = true;
			m_workers[i].m_cv.notify_one( );
		}
	}

	return true;
}

bool TaskScheduler::Wait( GroupHandle handle )
{
	if ( handle.m_groupIndex >= m_maxTaskGroup )
	{
		return false;
	}

	TaskGroup& group = m_taskGroups[handle.m_groupIndex];
	
	if ( handle.m_id < group.m_lastId )
	{
		return false;
	}

	if ( group.m_free || group.m_reference == 0 )
	{
		return false;
	}

	if ( CheckWorkerAffinity( GetThisThreadType( ), group.m_workerAffinity ) )
	{
		while ( true )
		{
			TaskBase* task = nullptr;
			while ( task == nullptr )
			{
				std::size_t head = group.m_head.load( );
				if ( group.m_head >= group.m_tasks.size( ) )
				{
					break;
				}

				if ( group.m_head.compare_exchange_strong( head, head + 1 ) )
				{
					std::shared_lock taskLock( group.m_taskLock );
					task = group.m_tasks[head];
				}
			}

			if ( task )
			{
				task->Execute( );
				--group.m_reference;
			}
			else
			{
				break;
			}
		}
	}

	while ( group.m_reference > 0 )
	{
		std::this_thread::yield( );
	}

	group.m_free = true;
	return true;
}

void TaskScheduler::WaitAll( )
{
	for ( std::size_t i = 0; i < m_maxTaskGroup; ++i )
	{
		Wait( GroupHandle{ i, m_taskGroups[i].m_lastId } );
	}
}

void TaskScheduler::ProcessThisThreadTask( )
{
	std::size_t threadType = GetThisThreadType( );

	for ( std::size_t i = 0; i < m_maxTaskGroup; ++i )
	{
		TaskGroup& group = m_taskGroups[i];
		if ( group.m_free || group.m_reference == 0 )
		{
			continue;
		}

		if ( CheckWorkerAffinity( threadType, group.m_workerAffinity ) == false )
		{
			continue;
		}

		while ( true )
		{
			TaskBase* task = nullptr;
			while ( task == nullptr )
			{
				std::size_t head = group.m_head.load( );
				if ( group.m_head >= group.m_tasks.size( ) )
				{
					break;
				}

				if ( group.m_head.compare_exchange_strong( head, head + 1 ) )
				{
					std::shared_lock taskLock( group.m_taskLock );
					task = group.m_tasks[head];
				}
			}

			if ( task )
			{
				task->Execute( );
				--group.m_reference;
			}
			else
			{
				break;
			}
		}

		group.m_free = true;
	}
}

bool TaskScheduler::IsComplete( GroupHandle handle ) const
{
	if ( handle.m_groupIndex >= m_maxTaskGroup )
	{
		return false;
	}

	TaskGroup& group = m_taskGroups[handle.m_groupIndex];

	return handle.m_id < group.m_lastId || group.m_free;
}

std::size_t TaskScheduler::GetThisThreadType( ) const
{
	std::thread::id thisThreadId = std::this_thread::get_id( );

	for ( std::size_t i = 0; i < m_workerCount; ++i )
	{
		if ( m_workerid[i] == thisThreadId )
		{
			return i;
		}
	}

	return m_workerCount;
}

TaskScheduler::TaskScheduler( )
{
	std::size_t workerCount = ( std::thread::hardware_concurrency( ) < 1 ) ? 1 : std::thread::hardware_concurrency( ) * 2 + 1;
	std::size_t groupCount = ( std::thread::hardware_concurrency( ) < 1 ) ? 4 : std::thread::hardware_concurrency( ) * 4;
	Initialize( groupCount, workerCount );
}

TaskScheduler::TaskScheduler( std::size_t workerCount )
{
	std::size_t groupCount = ( std::thread::hardware_concurrency( ) < 1 ) ? 4 : std::thread::hardware_concurrency( ) * 4;
	Initialize( groupCount, workerCount );
}

TaskScheduler::TaskScheduler( std::size_t groupCount, std::size_t workerCount )
{
	Initialize( groupCount, workerCount );
}

TaskScheduler::~TaskScheduler( )
{
	m_shutdown = true;

	for ( std::size_t i = 0; i < m_workerCount; ++i )
	{
		Worker& worker = m_workers[i];
		worker.m_wakeup = true;
		worker.m_cv.notify_one( );
		if ( worker.m_thread.joinable( ) )
		{
			worker.m_thread.join( );
		}
	}

	delete[] m_taskGroups;
	delete[] m_workers;
	delete[] m_workerid;
}

void TaskScheduler::Initialize( std::size_t groupCount, std::size_t workerCount )
{
	m_maxTaskGroup = groupCount;
	m_taskGroups = new TaskGroup[m_maxTaskGroup];

	for ( std::size_t i = 0; i < m_maxTaskGroup; ++i )
	{
		m_taskGroups[i].m_free = true;
		m_taskGroups[i].m_reference = 0;
		m_taskGroups[i].m_head = 0;
		m_taskGroups[i].m_lastId = 0;
		m_taskGroups[i].m_workerAffinity = std::numeric_limits<std::size_t>::max();
	}

	m_workerCount = workerCount;
	m_workers = new Worker[m_workerCount];
	m_workerid = new std::thread::id[m_workerCount];

	for ( std::size_t i = 0; i < m_workerCount; ++i )
	{
		m_workers[i].m_threadType = i;
		m_workers[i].m_wakeup = false;
		m_workers[i].m_thread = std::thread( WorkerThread, this, &m_workers[i] );
	}
}
