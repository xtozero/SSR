#include "D3D12NullDescriptor.h"

namespace agl
{
	void D3D12NullDescriptorStorage::Clear()
	{
		for ( D3D12DescriptorHeap& descriptor : m_nullDescriptors )
		{
			descriptor.Free();
		}
	}

	D3D12DescriptorHeap D3D12NullDescriptorStorage::m_nullDescriptors[MaxDescriptorType * MaxDescriptorUnderlyingType] = {};
}