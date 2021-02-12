#include "stdafx.h"
#include "D3D11SamplerState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

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

CD3D11SamplerState::CD3D11SamplerState( const SAMPLER_STATE_TRAIT& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}

void CD3D11SamplerState::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateSamplerState( &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}
