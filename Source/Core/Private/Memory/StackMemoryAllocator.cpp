#include "StackMemoryAllocator.h"

MemoryPage* PageAllocator::Allocate()
{
	return m_allocator.Allocate();
}

void PageAllocator::Deallocate( MemoryPage* memory )
{
	m_allocator.Deallocate( memory );
}
