#pragma once

#include <cstddef>
#include <limits>
#include <thread>

struct GroupHandle
{
	std::size_t m_groupIndex;
	std::size_t m_id;

	bool IsValid( ) const
	{
		return m_groupIndex != std::numeric_limits<std::size_t>::max( );
	}
};

inline bool operator==( const GroupHandle& lhs, const GroupHandle& rhs )
{
	return ( lhs.m_groupIndex == rhs.m_groupIndex ) && ( lhs.m_id == rhs.m_id );
}

inline bool operator!=( const GroupHandle& lhs, const GroupHandle& rhs )
{
	return !( lhs == rhs );
}

enum class TASK_TYPE
{
	WAITABLE,
	FIRE_AND_FORGET
};

class TaskBase
{
public:
	virtual void Execute( ) = 0;
	TASK_TYPE Type( )
	{
		return m_type;
	}

protected:
	explicit TaskBase( TASK_TYPE type ) : m_type( type ) {}
	virtual ~TaskBase( ) = default;
	TaskBase( const TaskBase& ) = delete;
	TaskBase& operator=( const TaskBase& ) = delete;
	TaskBase( TaskBase&& ) = delete;
	TaskBase& operator=( TaskBase&& ) = delete;

private:
	const TASK_TYPE m_type;
};

template <typename TaskStorageType>
class Task final : public TaskBase
{
public:
	virtual void Execute( ) override
	{
		reinterpret_cast<TaskStorageType*>( m_storage )->DoTask( );
		delete this;
	}

	template <typename... Args>
	static Task* Create( TASK_TYPE type, Args&&... args )
	{
		return new Task( type, args... );
	}

protected:
	template <typename... Args>
	Task( TASK_TYPE type, Args&&... args ) : TaskBase( type )
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

struct TaskGroup;
struct Worker;
class TaskScheduler;

void WorkerThread( TaskScheduler* scheduler, Worker* worker );

class TaskScheduler
{
public:
	GroupHandle GetTaskGroup( std::size_t workerAffinity = std::numeric_limits<std::size_t>::max() );

	bool Run( GroupHandle handle, TaskBase* task );

	bool Wait( GroupHandle handle );
	void WaitAll( );

	void ProcessThisThreadTask( );

	bool IsComplete( GroupHandle handle ) const;

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

	TaskGroup* m_taskGroups = nullptr;
	std::size_t m_maxTaskGroup = 4;
	std::size_t m_workerCount = 1;
	Worker* m_workers = nullptr;
	std::thread::id* m_workerid = nullptr;
	volatile bool m_shutdown = false;

	friend void WorkerThread( TaskScheduler* scheduler, Worker* worker );
};
