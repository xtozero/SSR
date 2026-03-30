#include "D3D11SamplerState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

using ::agl::ConvertToComparisionFunc;
using ::agl::ConvertToFilter;
using ::agl::ConvertToTextureAddress;
using ::agl::SamplerStateDesc;

namespace
{
	D3D11_SAMPLER_DESC ConvertToD3DDesc( const SamplerStateDesc& desc )
	{
		ColorF borderColor = desc.m_borderColor.ToColorF();

		return D3D11_SAMPLER_DESC{
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
	ID3D11SamplerState* D3D11SamplerState::Resource()
	{
		return m_samplerState;
	}

	const ID3D11SamplerState* D3D11SamplerState::Resource() const
	{
		return m_samplerState;
	}

	D3D11SamplerState::D3D11SamplerState( const SamplerStateDesc& desc ) : m_d3dDesc( ConvertToD3DDesc( desc ) ) {}

	void D3D11SamplerState::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateSamplerState( &m_d3dDesc, &m_samplerState ) );
		assert( result );
	}

	void D3D11SamplerState::FreeResource()
	{
		if ( m_samplerState )
		{
			m_samplerState->Release();
			m_samplerState = nullptr;
		}
	}
}
