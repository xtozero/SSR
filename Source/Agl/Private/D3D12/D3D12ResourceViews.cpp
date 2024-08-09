#include "D3D12ResourceViews.h"

#include "D3D12Api.h"
#include "D3D12BindlessManager.h"

namespace agl
{
	int32 D3D12ShaderResourceView::GetBindlessHandle() const
	{
		return m_bindlessHandle;
	}

	void D3D12ShaderResourceView::InitResource()
	{
		m_descriptorHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1 );
		D3D12Device().CreateShaderResourceView( m_d3d12Resource, &m_desc, m_descriptorHeap.GetCpuHandle().At() );

		m_bindlessHandle = D3D12BindlessMgr().AddResourceDescriptor( m_descriptorHeap.GetCpuHandle() );
	}

	void D3D12ShaderResourceView::FreeResource()
	{
		BaseClass::FreeResource();

		D3D12BindlessMgr().RemoveResourceDescriptor( m_bindlessHandle );
	}

	int32 D3D12UnorderedAccessView::GetBindlessHandle() const
	{
		return m_bindlessHandle;
	}

	void D3D12UnorderedAccessView::InitResource()
	{
		m_descriptorHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1 );
		D3D12Device().CreateUnorderedAccessView( m_d3d12Resource, nullptr, &m_desc, m_descriptorHeap.GetCpuHandle().At() );

		m_bindlessHandle = D3D12BindlessMgr().AddResourceDescriptor( m_descriptorHeap.GetCpuHandle() );
	}

	void D3D12UnorderedAccessView::FreeResource()
	{
		BaseClass::FreeResource();

		D3D12BindlessMgr().RemoveResourceDescriptor( m_bindlessHandle );
	}

	void D3D12RenderTargetView::InitResource()
	{
		m_descriptorHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1 );
		D3D12Device().CreateRenderTargetView( m_d3d12Resource, &m_desc, m_descriptorHeap.GetCpuHandle().At() );
	}

	void D3D12DepthStencilView::InitResource()
	{
		m_descriptorHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1 );
		D3D12Device().CreateDepthStencilView( m_d3d12Resource, &m_desc, m_descriptorHeap.GetCpuHandle().At() );
	}

	int32 D3D12ConstantBufferView::GetBindlessHandle() const
	{
		return m_bindlessHandle;
	}

	void D3D12ConstantBufferView::InitResource()
	{
		m_descriptorHeap = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1 );
		D3D12Device().CreateConstantBufferView( &m_desc, m_descriptorHeap.GetCpuHandle().At() );

		m_bindlessHandle = D3D12BindlessMgr().AddResourceDescriptor( m_descriptorHeap.GetCpuHandle() );
	}

	void D3D12ConstantBufferView::FreeResource()
	{
		BaseClass::FreeResource();

		D3D12BindlessMgr().RemoveResourceDescriptor( m_bindlessHandle );
	}
}
