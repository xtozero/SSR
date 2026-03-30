#include "D3D11BlendState.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

using ::agl::BlendStateDesc;
using ::agl::ConvertToBlend;
using ::agl::ConvertToBlendOp;
using ::agl::ConvertToColorWriteEnable;
using ::agl::MaxRendertagets;

namespace
{
	D3D11_BLEND_DESC ConvertToD3DDesc( const BlendStateDesc& desc )
	{
		D3D11_BLEND_DESC d3dDesc;

		d3dDesc.AlphaToCoverageEnable = desc.m_alphaToConverageEnable;
		d3dDesc.IndependentBlendEnable = desc.m_independentBlendEnable;

		for ( uint32 i = 0; i < MaxRendertagets; ++i )
		{
			d3dDesc.RenderTarget[i].BlendEnable = desc.m_renderTarget[i].m_blendEnable;
			d3dDesc.RenderTarget[i].SrcBlend = ConvertToBlend( desc.m_renderTarget[i].m_srcBlend );
			d3dDesc.RenderTarget[i].DestBlend = ConvertToBlend( desc.m_renderTarget[i].m_destBlend );
			d3dDesc.RenderTarget[i].BlendOp = ConvertToBlendOp( desc.m_renderTarget[i].m_blendOp );
			d3dDesc.RenderTarget[i].SrcBlendAlpha = ConvertToBlend( desc.m_renderTarget[i].m_srcBlendAlpha );
			d3dDesc.RenderTarget[i].DestBlendAlpha = ConvertToBlend( desc.m_renderTarget[i].m_destBlendAlpha );
			d3dDesc.RenderTarget[i].BlendOpAlpha = ConvertToBlendOp( desc.m_renderTarget[i].m_blendOpAlpha );
			d3dDesc.RenderTarget[i].RenderTargetWriteMask = ConvertToColorWriteEnable( desc.m_renderTarget[i].m_renderTargetWriteMask );
		}

		return d3dDesc;
	}
}

namespace agl
{
	ID3D11BlendState* D3D11BlendState::Resource()
	{
		return m_blendState;
	}

	const ID3D11BlendState* D3D11BlendState::Resource() const
	{
		return m_blendState;
	}

	const float* D3D11BlendState::GetBlendFactor() const
	{
		return m_blendFactor;
	}

	uint32 D3D11BlendState::SamplerMask() const
	{
		return m_sampleMask;
	}

	void D3D11BlendState::SetBlendFactor( const float( &blendFactor )[4] )
	{
		for ( uint32 i = 0; i < 4; ++i )
		{
			m_blendFactor[i] = blendFactor[i];
		}
	}

	void D3D11BlendState::SetSampleMask( uint32 sampleMask )
	{
		m_sampleMask = sampleMask;
	}

	D3D11BlendState::D3D11BlendState( const BlendStateDesc& desc ) : m_sampleMask( desc.m_sampleMask ), m_d3dDesc( ConvertToD3DDesc( desc ) ) {}

	void D3D11BlendState::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateBlendState( &m_d3dDesc, &m_blendState ) );
		assert( result );
	}

	void D3D11BlendState::FreeResource()
	{
		if ( m_blendState )
		{
			m_blendState->Release();
			m_blendState = nullptr;
		}
	}
}
