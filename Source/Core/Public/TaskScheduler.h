#ifndef _TASK_SCHEDULER_H_
#define _TASK_SCHEDULER_H_

#include <cstddef>

using WorkerFunc = void (*)( void* );
struct GroupHandle
{
	std::size_t m_groupIndex;
	std::size_t m_id;
};

inline bool operator==( const GroupHandle& lhs, const GroupHandle& rhs )
{
	return ( lhs.m_groupIndex == rhs.m_groupIndex ) && ( lhs.m_id == rhs.m_id );
}

inline bool operator!=( const GroupHandle& lhs, const GroupHandle& rhs )
{
	return !( lhs == rhs );
}

struct Task
{
	WorkerFunc m_func;
	void* m_context;
};

struct TaskGroup;
struct Worker;
class TaskScheduler;

void WorkerThread( TaskScheduler* scheduler, Worker* worker );

class TaskScheduler
{
public:
	GroupHandle GetTaskGroup( std::size_t reserveSize = 0 );

	bool Run( GroupHandle handle, WorkerFunc func, void* context );

	bool Wait( GroupHandle handle );
	void WaitAll( );

	bool IsComplete( GroupHandle handle ) const;

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
	volatile bool m_shutdown = false;

	friend void WorkerThread( TaskScheduler* scheduler, Worker* worker );
};

#endif
