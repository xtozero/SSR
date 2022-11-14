#include "D3D12ResourceViews.h"

#include "D3D12Api.h"

namespace agl
{
	void D3D12RenderTargetView::InitResource()
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = 1,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 0
		};
		[[maybe_unused]] HRESULT hr = D3D12Device().CreateDescriptorHeap( &heapDesc, IID_PPV_ARGS( &m_resource ) );
		assert( SUCCEEDED( hr ) );

		m_cpuHandle = m_resource->GetCPUDescriptorHandleForHeapStart();
		D3D12Device().CreateRenderTargetView( m_d3d12Resource, &m_desc, m_cpuHandle );
	}
}
