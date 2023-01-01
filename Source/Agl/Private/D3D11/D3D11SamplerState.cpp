#include "stdafx.h"
#include "D3D11SamplerState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

using ::agl::ConvertToComparisionFunc;
using ::agl::ConvertToFilter;
using ::agl::ConvertToTextureAddress;
using ::agl::SAMPLER_STATE_TRAIT;

namespace
{
	D3D11_SAMPLER_DESC ConvertTraitToDesc( const SAMPLER_STATE_TRAIT& trait )
	{
		return D3D11_SAMPLER_DESC{
			ConvertToFilter( trait.m_filter ),
			ConvertToTextureAddress( trait.m_addressU ),
			ConvertToTextureAddress( trait.m_addressV ),
			ConvertToTextureAddress( trait.m_addressW ),
			trait.m_mipLODBias,
			trait.m_maxAnisotropy,
			ConvertToComparisionFunc( trait.m_comparisonFunc ),
			{
				trait.m_borderColor[0],
				trait.m_borderColor[1],
				trait.m_borderColor[2],
				trait.m_borderColor[3]
			},
			trait.m_minLOD,
			trait.m_maxLOD
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

	D3D11SamplerState::D3D11SamplerState( const SAMPLER_STATE_TRAIT& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}

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
