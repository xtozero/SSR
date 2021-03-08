#include "stdafx.h"
#include "D3D11BlendState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

namespace
{
	D3D11_BLEND_DESC ConvertTraitToDesc( const BLEND_STATE_TRAIT& trait )
	{
		D3D11_BLEND_DESC desc;

		desc.AlphaToCoverageEnable = trait.m_alphaToConverageEnable;
		desc.IndependentBlendEnable = trait.m_independentBlendEnable;

		for ( int i = 0; i < 8; ++i )
		{
			desc.RenderTarget[i].BlendEnable = trait.m_renderTarget[i].m_blendEnable;
			desc.RenderTarget[i].SrcBlend = ConvertToBlend( trait.m_renderTarget[i].m_srcBlend );
			desc.RenderTarget[i].DestBlend = ConvertToBlend( trait.m_renderTarget[i].m_destBlend );
			desc.RenderTarget[i].BlendOp = ConvertToBlendOp( trait.m_renderTarget[i].m_blendOp );
			desc.RenderTarget[i].SrcBlendAlpha = ConvertToBlend( trait.m_renderTarget[i].m_srcBlendAlpha );
			desc.RenderTarget[i].DestBlendAlpha = ConvertToBlend( trait.m_renderTarget[i].m_destBlendAlpha );
			desc.RenderTarget[i].BlendOpAlpha = ConvertToBlendOp( trait.m_renderTarget[i].m_blendOpAlpha );
			desc.RenderTarget[i].RenderTargetWriteMask = ConvertToColorWriteEnable( trait.m_renderTarget[i].m_renderTargetWriteMask );
		}

		return desc;
	}
}

namespace aga
{
	D3D11BlendState::D3D11BlendState( const BLEND_STATE_TRAIT& trait ) : m_desc( ConvertTraitToDesc( trait ) ) {}

	void D3D11BlendState::InitResource( )
	{
		bool result = SUCCEEDED( D3D11Device( ).CreateBlendState( &m_desc, &m_blendState ) );
		assert( result );
	}

	void D3D11BlendState::FreeResource( )
	{
		if ( m_blendState )
		{
			m_blendState->Release( );
			m_blendState = nullptr;
		}
	}
}
