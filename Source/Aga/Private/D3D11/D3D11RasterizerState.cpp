#include "stdafx.h"
#include "D3D11RasterizerState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

namespace
{
	D3D11_RASTERIZER_DESC ConvertTraitToDesc( const RASTERIZER_STATE_TRAIT& trait )
	{
		return D3D11_RASTERIZER_DESC{
			ConvertToFillMode( trait.m_fillMode ),
			ConvertToCullMode( trait.m_cullMode ),
			trait.m_frontCounterClockwise,
			trait.m_depthBias,
			trait.m_depthBiasClamp,
			trait.m_slopeScaleDepthBias,
			trait.m_depthClipEnable,
			trait.m_scissorEnable,
			trait.m_multisampleEnalbe,
			trait.m_antialiasedLineEnable
		};
	}
}

CD3D11RasterizerState::CD3D11RasterizerState( const RASTERIZER_STATE_TRAIT& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}

void CD3D11RasterizerState::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateRasterizerState( &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}