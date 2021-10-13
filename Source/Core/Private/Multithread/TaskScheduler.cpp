#include "TaskScheduler.h"

#include <cassert>
#include <condition_variable>
#include <queue>
#include <mutex>

namespace
{
	bool CheckWorkerAffinity( size_t workerId, size_t workerAffinity )
	{
		assert( std::numeric_limits<size_t>::digits > workerId );
		return ( ( 1i64 << workerId ) & workerAffinity ) > 0;
	}
}

TaskHandle::TaskHandle( size_t queueId ) : m_queueId( queueId ), m_controlBlock( std::make_shared<TaskHandleControlBlock>( ) )
{
}

struct TaskQueue
{
	std::queue<TaskBase*> m_tasks;
	std::mutex m_taskLock;
	std::atomic<size_t> m_reference;
};

struct Worker
{
	size_t m_threadType;
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
			worker->m_wakeup = false;
		}

		while ( true )
		{
			if ( scheduler->m_shutdown )
			{
				return;
			}

			TaskQueue* queue = nullptr;
			TaskBase* task = nullptr;
			for ( size_t i = 0
				; ( i < scheduler->m_maxTaskQueue ) && ( task == nullptr )
				; ++i )
			{
				queue = &scheduler->m_taskQueues[i];
				if ( queue->m_reference == 0 )
				{
					continue;
				}

				std::unique_lock taskLock( queue->m_taskLock );
				if ( queue->m_tasks.empty( ) == false )
				{
					task = queue->m_tasks.front( );

					if ( CheckWorkerAffinity( worker->m_threadType, task->WorkerAffinity( ) ) == false )
					{
						task = nullptr;
						continue;
					}

					queue->m_tasks.pop( );
					break;
				}
			}

			if ( task == nullptr )
			{
				break;
			}

			task->Execute( );
			--queue->m_reference;
		}
	}
}

TaskHandle TaskScheduler::GetTaskGroup( )
{
	size_t workerId = m_workerCount;
	size_t minReference = (std::numeric_limits<size_t>::max)();
	for ( size_t i = m_workerCount; i < m_maxTaskQueue; ++i )
	{
		const TaskQueue& queue = m_taskQueues[i];
		if ( queue.m_reference < minReference )
		{
			workerId = i;
			minReference = queue.m_reference;
		}
	}

	return TaskHandle( workerId );
}

TaskHandle TaskScheduler::GetExclusiveTaskGroup( size_t workerId )
{
	return TaskHandle( workerId );
}

bool TaskScheduler::Run( TaskHandle handle )
{
	if ( handle.m_queueId >= m_maxTaskQueue )
	{
		return false;
	}

	TaskQueue& queue = m_taskQueues[handle.m_queueId];

	if ( handle.IsSubmitted( ) )
	{
		return false;
	}

	{
		std::unique_lock taskLock( queue.m_taskLock );
		for ( auto task : handle.m_controlBlock->m_tasks )
		{
			queue.m_tasks.push( task );
			++queue.m_reference;
		}
	}

	for ( size_t i = 0; i < m_workerCount; ++i )
	{
		std::unique_lock<std::mutex> lock( m_workers[i].m_lock );
		m_workers[i].m_wakeup = true;
		m_workers[i].m_cv.notify_one( );
	}

	handle.m_controlBlock->m_submitted = true;
	return true;
}

bool TaskScheduler::Wait( TaskHandle handle )
{
	if ( handle.m_queueId >= m_maxTaskQueue )
	{
		return false;
	}

	TaskQueue& queue = m_taskQueues[handle.m_queueId];
	
	if ( handle.IsSubmitted( ) == false )
	{
		return false;
	}

	if ( handle.IsCompleted( ) )
	{
		return false;
	}

	while ( true )
	{
		TaskBase* task = nullptr;
		{
			std::unique_lock taskLock( queue.m_taskLock );
			if ( queue.m_tasks.empty( ) == false )
			{
				task = queue.m_tasks.front( );

				if ( CheckWorkerAffinity( GetThisThreadType( ), task->WorkerAffinity( ) ) == false )
				{
					break;
				}

				queue.m_tasks.pop( );
			}
		}

		if ( task )
		{
			task->Execute( );
			--queue.m_reference;
		}
		else
		{
			break;
		}
	}

	while ( queue.m_reference > 0 )
	{
		std::this_thread::yield( );
	}

	return true;
}

void TaskScheduler::ProcessThisThreadTask( )
{
	size_t threadType = GetThisThreadType( );

	for ( size_t i = 0; i < m_maxTaskQueue; ++i )
	{
		TaskQueue& queue = m_taskQueues[i];
		if ( queue.m_reference == 0 )
		{
			continue;
		}

		while ( true )
		{
			TaskBase* task = nullptr;
			{
				std::unique_lock taskLock( queue.m_taskLock );
				if ( queue.m_tasks.empty( ) == false )
				{
					task = queue.m_tasks.front( );

					if ( CheckWorkerAffinity( threadType, task->WorkerAffinity( ) ) == false )
					{
						task = nullptr;
						break;
					}

					queue.m_tasks.pop( );
				}
			}

			if ( task )
			{
				task->Execute( );
				--queue.m_reference;
			}
			else
			{
				break;
			}
		}
	}
}

size_t TaskScheduler::GetThisThreadType( ) const
{
	std::thread::id thisThreadId = std::this_thread::get_id( );

	for ( size_t i = 0; i < m_workerCount; ++i )
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
	size_t workerCount = ( std::thread::hardware_concurrency( ) < 1 ) ? 1 : std::thread::hardware_concurrency( ) * 2 + 1;
	size_t queueCount = ( std::thread::hardware_concurrency( ) < 1 ) ? 4 : std::thread::hardware_concurrency( ) * 4;
	Initialize( queueCount, workerCount );
}

TaskScheduler::TaskScheduler( size_t workerCount )
{
	size_t queueCount = ( std::thread::hardware_concurrency( ) < 1 ) ? 4 : std::thread::hardware_concurrency( ) * 4;
	Initialize( queueCount, workerCount );
}

TaskScheduler::TaskScheduler( size_t queueCount, size_t workerCount )
{
	Initialize( queueCount, workerCount );
}

TaskScheduler::~TaskScheduler( )
{
	m_shutdown = true;

	for ( size_t i = 0; i < m_workerCount; ++i )
	{
		Worker& worker = m_workers[i];
		{
			std::unique_lock<std::mutex> lock( worker.m_lock );
			worker.m_wakeup = true;
		}
		worker.m_cv.notify_one( );
		if ( worker.m_thread.joinable( ) )
		{
			worker.m_thread.join( );
		}
	}

	delete[] m_taskQueues;
	delete[] m_workers;
	delete[] m_workerid;
}

void TaskScheduler::Initialize( size_t queueCount, size_t workerCount )
{
	m_maxTaskQueue = queueCount;
	m_taskQueues = new TaskQueue[m_maxTaskQueue];

	for ( size_t i = 0; i < m_maxTaskQueue; ++i )
	{
		m_taskQueues[i].m_reference = 0;
	}

	m_workerCount = workerCount;
	m_workers = new Worker[m_workerCount];
	m_workerid = new std::thread::id[m_workerCount];

	for ( size_t i = 0; i < m_workerCount; ++i )
	{
		m_workers[i].m_threadType = i;
		m_workers[i].m_wakeup = false;
		m_workers[i].m_thread = std::thread( WorkerThread, this, &m_workers[i] );
	}
}
