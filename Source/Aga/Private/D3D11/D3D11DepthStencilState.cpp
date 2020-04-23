#include "stdafx.h"
#include "D3D11DepthStencilState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

namespace
{
	D3D11_DEPTH_STENCIL_DESC ConvertTraitToDesc( const DEPTH_STENCIL_STATE_TRAIT& trait )
	{
		return D3D11_DEPTH_STENCIL_DESC{
			trait.m_depthEnable,
			ConvertToDepthWriteMask( trait.m_depthWriteMode ),
			ConvertToComparisionFunc( trait.m_depthFunc ),
			trait.m_stencilEnable,
			trait.m_stencilReadMask,
			trait.m_stencilWriteMask,
			{
				ConvertToStencilOp( trait.m_frontFace.m_failOp ),
				ConvertToStencilOp( trait.m_frontFace.m_depthFailOp ),
				ConvertToStencilOp( trait.m_frontFace.m_passOp ),
				ConvertToComparisionFunc( trait.m_frontFace.m_func )
			},
			{
				ConvertToStencilOp( trait.m_backFace.m_failOp ),
				ConvertToStencilOp( trait.m_backFace.m_depthFailOp ),
				ConvertToStencilOp( trait.m_backFace.m_passOp ),
				ConvertToComparisionFunc( trait.m_backFace.m_func )
			}
		};
	}
}

void CD3D11DepthStencilState::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateDepthStencilState( &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11DepthStencilState::CD3D11DepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}
