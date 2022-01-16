#include "StackMemoryAllocator.h"

MemoryPage* PageAllocator::Allocate()
{
	return m_allocator.Allocate( 1 );
}

void PageAllocator::Deallocate( MemoryPage* memory )
{
	m_allocator.Deallocate( memory, 1 );
}
