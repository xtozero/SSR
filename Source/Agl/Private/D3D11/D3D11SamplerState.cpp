#include "stdafx.h"
#include "D3D11SamplerState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

using ::agl::ConvertToComparisionFunc;
using ::agl::ConvertToFilter;
using ::agl::ConvertToTextureAddress;
using ::agl::SamplerStateTrait;

namespace
{
	D3D11_SAMPLER_DESC ConvertTraitToDesc( const SamplerStateTrait& trait )
	{
		ColorF borderColor = trait.m_borderColor.ToColorF();

		return D3D11_SAMPLER_DESC{
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
	ID3D11SamplerState* D3D11SamplerState::Resource()
	{
		return m_samplerState;
	}

	const ID3D11SamplerState* D3D11SamplerState::Resource() const
	{
		return m_samplerState;
	}

	D3D11SamplerState::D3D11SamplerState( const SamplerStateTrait& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}

	void D3D11SamplerState::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateSamplerState( &m_desc, &m_samplerState ) );
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
