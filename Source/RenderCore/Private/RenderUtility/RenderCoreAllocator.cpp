#include "RenderCoreAllocator.h"

namespace rendercore
{
	StackAllocator& Allocator()
	{
		static StackAllocator allocator;
		return allocator;
	}
}