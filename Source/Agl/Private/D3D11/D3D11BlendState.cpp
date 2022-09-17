#include "stdafx.h"
#include "D3D11BlendState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

using ::agl::BLEND_STATE_TRAIT;
using ::agl::ConvertToBlend;
using ::agl::ConvertToBlendOp;
using ::agl::ConvertToColorWriteEnable;
using ::agl::MAX_RENDER_TARGET;

namespace
{
	D3D11_BLEND_DESC ConvertTraitToDesc( const BLEND_STATE_TRAIT& trait )
	{
		D3D11_BLEND_DESC desc;

		desc.AlphaToCoverageEnable = trait.m_alphaToConverageEnable;
		desc.IndependentBlendEnable = trait.m_independentBlendEnable;

		for ( uint32 i = 0; i < MAX_RENDER_TARGET; ++i )
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

namespace agl
{
	ID3D11BlendState* D3D11BlendState::Resource( )
	{
		return m_blendState;
	}

	const ID3D11BlendState* D3D11BlendState::Resource( ) const
	{
		return m_blendState;
	}

	const float* D3D11BlendState::GetBlendFactor( ) const
	{
		return m_blendFactor;
	}

	uint32 D3D11BlendState::SamplerMask( ) const
	{
		return m_sampleMask;
	}

	void D3D11BlendState::SetBlendFactor( const float( &blendFactor )[4] )
	{
		constexpr uint32 size = std::extent_v<decltype( blendFactor )>;
		for ( uint32 i = 0; i < size; ++i )
		{
			m_blendFactor[i] = blendFactor[i];
		}
	}

	void D3D11BlendState::SetSampleMask( uint32 sampleMask )
	{
		m_sampleMask = sampleMask;
	}

	D3D11BlendState::D3D11BlendState( const BLEND_STATE_TRAIT& trait ) : m_desc( ConvertTraitToDesc( trait ) ), m_sampleMask( trait.m_sampleMask ) {}

	void D3D11BlendState::InitResource( )
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device( ).CreateBlendState( &m_desc, &m_blendState ) );
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
