#include "D3D12SamplerState.h"

#include "D3D12Api.h"
#include "D3D12FlagConvertor.h"

using ::agl::ConvertToComparisionFunc;
using ::agl::ConvertToFilter;
using ::agl::ConvertToTextureAddress;
using ::agl::SamplerStateDesc;

namespace
{
	D3D12_SAMPLER_DESC ConvertToD3DDesc( const SamplerStateDesc& desc )
	{
		ColorF borderColor = desc.m_borderColor.ToColorF();

		return D3D12_SAMPLER_DESC{
			.Filter = ConvertToFilter( desc.m_filter ),
			.AddressU = ConvertToTextureAddress( desc.m_addressU ),
			.AddressV = ConvertToTextureAddress( desc.m_addressV ),
			.AddressW = ConvertToTextureAddress( desc.m_addressW ),
			.MipLODBias = desc.m_mipLODBias,
			.MaxAnisotropy = desc.m_maxAnisotropy,
			.ComparisonFunc = ConvertToComparisionFunc( desc.m_comparisonFunc ),
			.BorderColor = {
				borderColor[0],
				borderColor[1],
				borderColor[2],
				borderColor[3]
			},
			.MinLOD = desc.m_minLOD,
			.MaxLOD = desc.m_maxLOD
		};
	}
}

namespace agl
{
	int32 D3D12SamplerState::GetBindlessHandle() const
	{
		return m_bindlessHandle;
	}

	const D3D12DescriptorHeap& D3D12SamplerState::Resource() const
	{
		return m_samplerState;
	}

	D3D12SamplerState::D3D12SamplerState( const SamplerStateDesc& desc )
		: m_d3dDesc( ConvertToD3DDesc( desc ) )
	{
	}

	void D3D12SamplerState::InitResource()
	{
		m_samplerState = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1 );
		D3D12Device().CreateSampler( &m_d3dDesc, m_samplerState.GetCpuHandle().At() );

		m_bindlessHandle = D3D12BindlessMgr().AddSamplerDescriptor( m_samplerState.GetCpuHandle() );
	}

	void D3D12SamplerState::FreeResource()
	{
		std::destroy_at( &m_samplerState );

		D3D12BindlessMgr().RemoveSamplerDescriptor( m_bindlessHandle );
	}
}
