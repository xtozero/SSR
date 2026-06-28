#include "D3D12ResourceViews.h"

#include "D3D12Api.h"
#include "D3D12BindlessManager.h"

namespace agl
{
	int32 D3D12ShaderResourceView::GetBindlessHandle() const
	{
		return m_bindlessHandle;
	}

	void D3D12ShaderResourceView::UpdateTextureMips( ID3D12Resource* resource, uint32 mipLevels )
	{
		switch ( m_d3dDesc.ViewDimension )
		{
		case D3D12_SRV_DIMENSION_TEXTURE1D:
			m_d3dDesc.Texture1D.MipLevels = mipLevels;
			break;
		case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
			m_d3dDesc.Texture1DArray.MipLevels = mipLevels;
			break;
		case D3D12_SRV_DIMENSION_TEXTURE2D:
			m_d3dDesc.Texture2D.MipLevels = mipLevels;
			break;
		case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
			m_d3dDesc.Texture2DArray.MipLevels = mipLevels;
			break;
		case D3D12_SRV_DIMENSION_TEXTURE3D:
			m_d3dDesc.Texture3D.MipLevels = mipLevels;
			break;
		case D3D12_SRV_DIMENSION_TEXTURECUBE:
			m_d3dDesc.TextureCube.MipLevels = mipLevels;
			break;
		case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
			m_d3dDesc.TextureCubeArray.MipLevels = mipLevels;
			break;
		default:
			assert( false && "Unsupported SRV dimension in D3D12ShaderResourceView::UpdateTextureMips()" );
			return;
		}

		m_d3d12Resource = resource;
		D3D12Device().CreateShaderResourceView( m_d3d12Resource, &m_d3dDesc, m_descriptor.m_cpuHandle.At() );

		D3D12BindlessMgr().RemoveResourceDescriptor( m_bindlessHandle );
		m_bindlessHandle = D3D12BindlessMgr().AddResourceDescriptor( m_descriptor.m_cpuHandle );
	}

	void D3D12ShaderResourceView::InitResource()
	{
		m_descriptor = D3D12DescriptorPoolForView().Acquire( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );

		ID3D12Resource* resource = ( m_d3dDesc.ViewDimension == D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE ) ? nullptr : m_d3d12Resource;
		D3D12Device().CreateShaderResourceView( resource, &m_d3dDesc, m_descriptor.m_cpuHandle.At() );

		m_bindlessHandle = D3D12BindlessMgr().AddResourceDescriptor( m_descriptor.m_cpuHandle );
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
		m_descriptor = D3D12DescriptorPoolForView().Acquire( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		D3D12Device().CreateUnorderedAccessView( m_d3d12Resource, nullptr, &m_d3dDesc, m_descriptor.m_cpuHandle.At() );

		m_bindlessHandle = D3D12BindlessMgr().AddResourceDescriptor( m_descriptor.m_cpuHandle );
	}

	void D3D12UnorderedAccessView::FreeResource()
	{
		BaseClass::FreeResource();

		D3D12BindlessMgr().RemoveResourceDescriptor( m_bindlessHandle );
	}

	D3D12RenderTargetView::D3D12RenderTargetView( GraphicsApiResource* owner, ID3D12Resource* d3d12Resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc, const ColorF& clearColor )
		: BaseClass( owner, d3d12Resource, desc )
		, m_clearColor( clearColor )
	{
	}

	ColorF D3D12RenderTargetView::GetClearColor() const
	{
		return m_clearColor;
	}

	void D3D12RenderTargetView::InitResource()
	{
		m_descriptor = D3D12DescriptorPoolForView().Acquire( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
		D3D12Device().CreateRenderTargetView( m_d3d12Resource, &m_d3dDesc, m_descriptor.m_cpuHandle.At() );
	}

	D3D12DepthStencilView::D3D12DepthStencilView( GraphicsApiResource* owner, ID3D12Resource* d3d12Resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc, float depthValue, uint8 stencilValue )
		: BaseClass( owner, d3d12Resource, desc )
		, m_depthClearValue( depthValue )
		, m_stencilClearValue( stencilValue )
	{
	}

	float D3D12DepthStencilView::GetDepthClearValue() const
	{
		return m_depthClearValue;
	}

	uint8 D3D12DepthStencilView::GetStencilClearValue() const
	{
		return m_stencilClearValue;
	}

	void D3D12DepthStencilView::InitResource()
	{
		m_descriptor = D3D12DescriptorPoolForView().Acquire( D3D12_DESCRIPTOR_HEAP_TYPE_DSV );
		D3D12Device().CreateDepthStencilView( m_d3d12Resource, &m_d3dDesc, m_descriptor.m_cpuHandle.At() );
	}

	int32 D3D12ConstantBufferView::GetBindlessHandle() const
	{
		return m_bindlessHandle;
	}

	void D3D12ConstantBufferView::InitResource()
	{
		m_descriptor = D3D12DescriptorPoolForView().Acquire( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		D3D12Device().CreateConstantBufferView( &m_d3dDesc, m_descriptor.m_cpuHandle.At() );

		m_bindlessHandle = D3D12BindlessMgr().AddResourceDescriptor( m_descriptor.m_cpuHandle );
	}

	void D3D12ConstantBufferView::FreeResource()
	{
		BaseClass::FreeResource();

		D3D12BindlessMgr().RemoveResourceDescriptor( m_bindlessHandle );
	}
}
