#include "D3D12ResourceViews.h"

#include "D3D12Api.h"

namespace agl
{
	void D3D12RenderTargetView::InitResource()
	{
		m_descriptorHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1 );
		D3D12Device().CreateRenderTargetView( m_d3d12Resource, &m_desc, m_descriptorHeap.GetCpuHandle().At() );
	}

	void D3D12ConstantBufferView::InitResource()
	{
		m_descriptorHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1 );
		D3D12Device().CreateConstantBufferView( &m_desc, m_descriptorHeap.GetCpuHandle().At() );
	}
}
