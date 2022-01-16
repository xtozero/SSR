#pragma once
#include "StackMemoryAllocator.h"

#include "Multithread/TaskScheduler.h"

#include <cassert>
#include <vector>

namespace rendercore
{
	RENDERCORE_FUNC_DLL StackAllocator& Allocator();

	template <typename T>
	class RenderCoreAllocator
	{
	public:
		using value_type = T;
		using size_type = size_t;
		using difference_type = uptrint;
		using propagate_on_container_move_assignment = std::false_type;

		T* allocate( size_t n )
		{
			assert( IsInRenderThread() );
			return Allocator().Allocate<T>( n );
		}

		void deallocate( [[maybe_unused]] T* p, [[maybe_unused]] size_t n ) { /*Do Nothing*/ }

		constexpr RenderCoreAllocator() noexcept {}
		constexpr RenderCoreAllocator( const RenderCoreAllocator& ) noexcept = default;
		template <class Other>
		constexpr RenderCoreAllocator( const RenderCoreAllocator<Other>& ) noexcept {}
		~RenderCoreAllocator() = default;
		RenderCoreAllocator& operator=( const RenderCoreAllocator& ) = default;
	};

	template <typename T>
	using VectorSingleFrame = std::vector<T, RenderCoreAllocator<T>>;
}