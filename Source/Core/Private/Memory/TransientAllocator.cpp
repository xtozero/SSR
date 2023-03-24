#include "TransientAllocator.h"

StackAllocator& TransientAllocators::GetAllocator( ThreadType threadType )
{
	return m_allocators[static_cast<int32>( threadType )];
}
