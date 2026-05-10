#pragma once

#include "Multithread/TaskScheduler.h"
#include "SizedTypes.h"
#include "StackMemoryAllocator.h"

#include <cassert>
#include <map>
#include <set>
#include <vector>

class TransientAllocators
{
public:
	StackAllocator& GetAllocator( ThreadType threadType );

private:
	StackAllocator m_allocators[static_cast<int32>(ThreadType::NumThread)];
};

template <typename T, ThreadType LocalThreadType>
class TransientAllocator
{
public:
	using value_type = T;
	using size_type = size_t;
	using difference_type = ptrint;
	using propagate_on_container_move_assignment = std::true_type;

	[[nodiscard]] constexpr T* allocate( size_t n )
	{
		static auto taskScheduler = GetInterface<ITaskScheduler>();
		assert( static_cast<ThreadType>( taskScheduler->GetThisThreadType() ) == LocalThreadType );

		static auto threadLocalAllocators = GetInterface<TransientAllocators>();
		// To fix error C2131, declare the variable as a pointer
		static StackAllocator* allocator = &threadLocalAllocators->GetAllocator( LocalThreadType );

		return allocator->Allocate<T>( n );
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
using RenderFrameArray = std::vector<T, TransientAllocator<T, ThreadType::RenderThread>>;

template <typename K, typename V, typename P = std::less<K>>
using RenderFrameMap = std::map<K, V, P, TransientAllocator<std::pair<const K, V>, ThreadType::RenderThread>>;

template <typename K, typename P = std::less<K>>
using RenderFrameSet = std::set<K, P, TransientAllocator<K, ThreadType::RenderThread>>;

template <typename T, ThreadType LocalThreadType>
class ResetFrameArrayScope
{
public:
	explicit ResetFrameArrayScope( std::vector<T, TransientAllocator<T, LocalThreadType>>& container )
		: m_container( &container )
	{
		std::destroy_at( m_container );
	}

	~ResetFrameArrayScope()
	{
		std::construct_at( m_container );
	}

private:
	std::vector<T, TransientAllocator<T, LocalThreadType>>* m_container = nullptr;
};