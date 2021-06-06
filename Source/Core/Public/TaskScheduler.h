#pragma once

#include <atomic>
#include <cstddef>
#include <limits>
#include <memory>
#include <thread>
#include <vector>


struct TaskHandleControlBlock
{
	std::vector<class TaskBase*> m_tasks;
	std::atomic<int> m_executed = 0;
	std::atomic<bool> m_submitted = false;
};

class TaskBase
{
public:
	virtual void Execute( ) = 0;

	std::size_t WorkerAffinity( ) const
	{
		return m_workerAffinity;
	}

protected:
	explicit TaskBase( std::size_t workerAffinity ) : m_workerAffinity( workerAffinity ) {}
	virtual ~TaskBase( ) = default;
	TaskBase( const TaskBase& ) = delete;
	TaskBase& operator=( const TaskBase& ) = delete;
	TaskBase( TaskBase&& ) = delete;
	TaskBase& operator=( TaskBase&& ) = delete;

	std::shared_ptr<TaskHandleControlBlock> m_controlBlock;

private:
	std::size_t m_workerAffinity;

	friend class TaskHandle;
};

class TaskHandle
{
public:
	void AddTask( class TaskBase* task )
	{
		task->m_controlBlock = m_controlBlock;

		auto& tasks = m_controlBlock->m_tasks;
		tasks.push_back( task );
	}

	bool IsCompleted( ) const
	{
		if ( IsSubmitted( ) == false )
		{
			return false;
		}

		std::size_t totalTask = m_controlBlock->m_tasks.size( );
		return totalTask == m_controlBlock->m_executed;
	}

	bool IsSubmitted( ) const
	{
		return m_controlBlock->m_submitted;
	}

	explicit TaskHandle( std::size_t queueId );
	TaskHandle( ) = default;
	~TaskHandle( ) = default;
	TaskHandle( const TaskHandle& ) = default;
	TaskHandle& operator=( const TaskHandle& ) = default;
	TaskHandle( TaskHandle&& ) = default;
	TaskHandle& operator=( TaskHandle&& ) = default;

	std::size_t m_queueId = ( std::numeric_limits<std::size_t>::max )( );
	std::shared_ptr<TaskHandleControlBlock> m_controlBlock;
};

template <typename TaskStorageType>
class Task final : public TaskBase
{
public:
	virtual void Execute( ) override
	{
		reinterpret_cast<TaskStorageType*>( m_storage )->DoTask( );
		++m_controlBlock->m_executed;
		delete this;
	}

	template <typename... Args>
	static Task* Create( std::size_t workerAffinity, Args&&... args )
	{
		return new Task( workerAffinity, args... );
	}

protected:
	template <typename... Args>
	Task( std::size_t workerAffinity, Args&&... args ) : TaskBase( workerAffinity )
	{
		new ( &m_storage )TaskStorageType( args... );
	}

	~Task( )
	{
		reinterpret_cast<TaskStorageType*>( m_storage )->~TaskStorageType( );
	}

	Task( const Task& ) = delete;
	Task& operator=( const Task& ) = delete;
	Task( Task&& ) = delete;
	Task& operator=( Task&& ) = delete;

private:
	unsigned char m_storage[sizeof( TaskStorageType )];
};

struct TaskQueue;
struct Worker;
class TaskScheduler;

void WorkerThread( TaskScheduler* scheduler, Worker* worker );

class TaskScheduler
{
public:
	TaskHandle GetTaskGroup( );
	TaskHandle GetExclusiveTaskGroup( std::size_t workerId );

	bool Run( TaskHandle handle );

	bool Wait( TaskHandle handle );

	void ProcessThisThreadTask( );

	std::size_t GetThisThreadType( ) const;

	TaskScheduler( );
	TaskScheduler( std::size_t workerCount );
	TaskScheduler( std::size_t groupCount, std::size_t workerCount );
	~TaskScheduler( );
	TaskScheduler( const TaskScheduler& ) = delete;
	TaskScheduler& operator=( const TaskScheduler& ) = delete;
	TaskScheduler( TaskScheduler&& ) = delete;
	TaskScheduler& operator=( TaskScheduler&& ) = delete;

private:
	void Initialize( std::size_t groupCount, std::size_t workerCount );

	TaskQueue* m_taskQueues = nullptr;
	std::size_t m_maxTaskQueue = 4;
	std::size_t m_workerCount = 1;
	Worker* m_workers = nullptr;
	std::thread::id* m_workerid = nullptr;
	volatile bool m_shutdown = false;

	friend void WorkerThread( TaskScheduler* scheduler, Worker* worker );
};
