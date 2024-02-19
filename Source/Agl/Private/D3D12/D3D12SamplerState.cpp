#include "D3D12SamplerState.h"

#include "D3D12Api.h"
#include "D3D12FlagConvertor.h"

using ::agl::ConvertToComparisionFunc;
using ::agl::ConvertToFilter;
using ::agl::ConvertToTextureAddress;
using ::agl::SamplerStateTrait;

namespace
{
	D3D12_SAMPLER_DESC ConvertTraitToDesc( const SamplerStateTrait& trait )
	{
		ColorF borderColor = trait.m_borderColor.ToColorF();

		return D3D12_SAMPLER_DESC{
			.Filter = ConvertToFilter( trait.m_filter ),
			.AddressU = ConvertToTextureAddress( trait.m_addressU ),
			.AddressV = ConvertToTextureAddress( trait.m_addressV ),
			.AddressW = ConvertToTextureAddress( trait.m_addressW ),
			.MipLODBias = trait.m_mipLODBias,
			.MaxAnisotropy = trait.m_maxAnisotropy,
			.ComparisonFunc = ConvertToComparisionFunc( trait.m_comparisonFunc ),
			.BorderColor = {
				borderColor[0],
				borderColor[1],
				borderColor[2],
				borderColor[3]
			},
			.MinLOD = trait.m_minLOD,
			.MaxLOD = trait.m_maxLOD
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

	D3D12SamplerState::D3D12SamplerState( const SamplerStateTrait& trait )
		: m_desc( ConvertTraitToDesc( trait ) ) 
	{
	}

	void D3D12SamplerState::InitResource()
	{
		m_samplerState = D3D12DescriptorHeapAllocator::GetInstance().AllocCpuDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1 );
		D3D12Device().CreateSampler( &m_desc, m_samplerState.GetCpuHandle().At() );

		m_bindlessHandle = D3D12BindlessMgr().AddSamplerDescriptor( m_samplerState.GetCpuHandle() );
	}

	void D3D12SamplerState::FreeResource()
	{
		std::destroy_at( &m_samplerState );

		D3D12BindlessMgr().RemoveSamplerDescriptor( m_bindlessHandle );
	}
}
