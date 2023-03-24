#pragma once
#include "StackMemoryAllocator.h"

#include "Multithread/TaskScheduler.h"
#include "SizedTypes.h"

#include <cassert>
#include <vector>

class TransientAllocators
{
public:
	StackAllocator& GetAllocator( ThreadType threadType );

private:
	StackAllocator m_allocators[static_cast<int32>(ThreadType::WorkerThreadCount)];
};

template <typename T, ThreadType LocalThreadType>
class TransientAllocator
{
public:
	using value_type = T;
	using size_type = size_t;
	using difference_type = uptrint;
	using propagate_on_container_move_assignment = std::false_type;

	[[nodiscard]] constexpr T* allocate( size_t n )
	{
		static auto taskScheduler = GetInterface<ITaskScheduler>();
		assert( static_cast<ThreadType>( taskScheduler->GetThisThreadType() ) == LocalThreadType );

		static auto threadLocalAllocators = GetInterface<TransientAllocators>();
		static StackAllocator& allocator = threadLocalAllocators->GetAllocator( LocalThreadType );

		return allocator.Allocate<T>( n );
	}

	void deallocate( [[maybe_unused]] T* p, [[maybe_unused]] size_t n ) { /*Do Nothing*/ }

	template<typename Other>
	struct rebind {
		using other = TransientAllocator<Other, LocalThreadType>;
	};

	constexpr TransientAllocator() = default;
	constexpr TransientAllocator( const TransientAllocator& ) = default;
	template <class Other>
	constexpr TransientAllocator( const TransientAllocator<Other, LocalThreadType>& ) noexcept {}
	~TransientAllocator() = default;
	TransientAllocator& operator=( const TransientAllocator& ) = default;
};

template <ThreadType LocalThreadType>
StackAllocator& GetTransientAllocator()
{
	static auto threadLocalAllocators = GetInterface<TransientAllocators>();
	static StackAllocator& allocator = threadLocalAllocators->GetAllocator( LocalThreadType );

	return allocator;
}

template <typename T>
using RenderThreadFrameData = std::vector<T, TransientAllocator<T, ThreadType::RenderThread>>;
