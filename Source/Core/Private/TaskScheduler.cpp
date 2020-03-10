#include "TaskScheduler.h"

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

struct TaskGroup
{
	std::vector<Task> m_tasks;
	std::mutex m_taskLock;
	std::atomic<bool> m_free;
	std::atomic<std::size_t> m_reference;
	std::atomic<std::size_t> m_lastId;
	std::size_t m_head;
};

struct Worker
{
	std::thread m_thread;
	std::mutex m_lock;
	std::condition_variable m_cv;
	std::atomic<bool> m_wakeup;
};

void WorkerThread( TaskScheduler* scheduler, Worker* worker )
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( worker->m_lock );
		worker->m_cv.wait( lock, [&worker]( ) { return worker->m_wakeup.load( ); } );
		worker->m_wakeup = false;

		while ( true )
		{
			if ( scheduler->m_shutdown )
			{
				return;
			}

			TaskGroup* group = nullptr;
			Task task = { nullptr, nullptr };
			for ( std::size_t i = 0; i < scheduler->m_maxTaskGroup; ++i )
			{
				group = &scheduler->m_taskGroups[i];
				if ( group->m_free || group->m_reference == 0 )
				{
					continue;
				}

				std::lock_guard<std::mutex> taskLock( group->m_taskLock );
				if ( group->m_head < group->m_tasks.size( ) )
				{
					task = group->m_tasks[group->m_head++];
					break;
				}
			}

			if ( task.m_func )
			{
				task.m_func( task.m_context );
				--group->m_reference;
			}
		}
	}
}

GroupHandle TaskScheduler::GetTaskGroup( std::size_t reserveSize )
{
	for ( std::size_t i = 0; i < m_maxTaskGroup; ++i )
	{
		TaskGroup& group = m_taskGroups[i];
		bool expected = true;
		if ( group.m_free.compare_exchange_strong( expected, false ) )
		{
			group.m_head = 0;
			std::lock_guard<std::mutex> taskLock( group.m_taskLock );
			group.m_tasks.clear( );
			group.m_tasks.reserve( reserveSize );
			return GroupHandle{ i, ++group.m_lastId };
		}
	}

	return GroupHandle{ std::numeric_limits<std::size_t>::max( ), 0 };
}

bool TaskScheduler::Run( GroupHandle handle, WorkerFunc func, void* context )
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
		std::lock_guard<std::mutex> taskLock( group.m_taskLock );
		group.m_tasks.push_back( { func, context } );
	}
	
	++group.m_reference;
	
	for ( std::size_t i = 0; i < m_workerCount; ++i )
	{
		m_workers[i].m_wakeup = true;
		m_workers[i].m_cv.notify_one( );
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

	while ( true )
	{
		Task* task = nullptr;
		{
			std::lock_guard<std::mutex> taskLock( group.m_taskLock );
			if ( group.m_head < group.m_tasks.size( ) )
			{
				task = &group.m_tasks[group.m_head++];
			}
		}

		if ( task )
		{
			task->m_func( task->m_context );
			--group.m_reference;
		}
		else
		{
			break;
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

bool TaskScheduler::IsComplete( GroupHandle handle ) const
{
	if ( handle.m_groupIndex >= m_maxTaskGroup )
	{
		return false;
	}

	TaskGroup& group = m_taskGroups[handle.m_groupIndex];

	return handle.m_id < group.m_lastId || group.m_free;
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
	}

	m_workerCount = workerCount;
	m_workers = new Worker[m_workerCount];

	for ( std::size_t i = 0; i < m_workerCount; ++i )
	{
		m_workers[i].m_wakeup = false;
		m_workers[i].m_thread = std::thread( WorkerThread, this, &m_workers[i] );
	}
}
