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

	const D3D12CpuDescriptorHandle& D3D12SamplerState::GetCpuHandle() const
	{
		return m_descriptor.m_cpuHandle;
	}

	D3D12SamplerState::D3D12SamplerState( const SamplerStateDesc& desc )
		: m_d3dDesc( ConvertToD3DDesc( desc ) )
	{
	}

	void D3D12SamplerState::InitResource()
	{
		m_descriptor = D3D12DescriptorPoolForView().Acquire( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER );
		D3D12Device().CreateSampler( &m_d3dDesc, m_descriptor.m_cpuHandle.At() );

		m_bindlessHandle = D3D12BindlessMgr().AddSamplerDescriptor( m_descriptor.m_cpuHandle );
	}

	void D3D12SamplerState::FreeResource()
	{
		D3D12DescriptorPoolForView().Release( m_descriptor );

		D3D12BindlessMgr().RemoveSamplerDescriptor( m_bindlessHandle );
	}
}
