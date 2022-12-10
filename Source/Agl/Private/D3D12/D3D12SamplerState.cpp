#include "D3D12SamplerState.h"

#include "D3D12Api.h"
#include "D3D12FlagConvertor.h"

using ::agl::ConvertToComparisionFunc;
using ::agl::ConvertToFilter;
using ::agl::ConvertToTextureAddress;
using ::agl::SAMPLER_STATE_TRAIT;

namespace
{
	D3D12_SAMPLER_DESC ConvertTraitToDesc( const SAMPLER_STATE_TRAIT& trait )
	{
		return D3D12_SAMPLER_DESC{
			.Filter = ConvertToFilter( trait.m_filter ),
			.AddressU = ConvertToTextureAddress( trait.m_addressU ),
			.AddressV = ConvertToTextureAddress( trait.m_addressV ),
			.AddressW = ConvertToTextureAddress( trait.m_addressW ),
			.MipLODBias = trait.m_mipLODBias,
			.MaxAnisotropy = trait.m_maxAnisotropy,
			.ComparisonFunc = ConvertToComparisionFunc( trait.m_comparisonFunc ),
			.BorderColor = {
				trait.m_borderColor[0],
				trait.m_borderColor[1],
				trait.m_borderColor[2],
				trait.m_borderColor[3]
			},
			.MinLOD = trait.m_minLOD,
			.MaxLOD = trait.m_maxLOD
		};
	}
}

namespace agl
{
	D3D12SamplerState::D3D12SamplerState( const SAMPLER_STATE_TRAIT& trait ) 
		: m_desc( ConvertTraitToDesc( trait ) ) 
	{
	}

	void D3D12SamplerState::InitResource()
	{
		m_samplerState = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1 );
		D3D12Device().CreateSampler( &m_desc, m_samplerState.GetCpuHandle().At() );
	}

	void D3D12SamplerState::FreeResource()
	{
		std::destroy_at( &m_samplerState );
	}
}
