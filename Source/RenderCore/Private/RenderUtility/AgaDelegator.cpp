#include "stdafx.h"
#include "AgaDelegator.h"
#include "Shader.h"

AgaDelegator g_agaDelegator;

void AgaDelegator::BootUp( IAga* pAga )
{
	m_aga = pAga;
	m_defaultConstants.BootUp( );
}

void AgaDelegator::Shutdown( )
{
	m_defaultConstants.Shutdown( );
}

void AgaDelegator::Dispatch( UINT x, UINT y, UINT z )
{
	ComputeShader empty;
	m_defaultConstants.Commit( empty );

	m_aga->Dispatch( x, y, z );

	BindShader( empty );
}

BlendState AgaDelegator::FindOrCreate( const BlendOption& option )
{
	auto found = m_blendStates.find( option );
	if ( found == m_blendStates.end( ) )
	{
		BLEND_STATE_TRAIT trait;
		trait.m_alphaToConverageEnable = option.m_alphaToConverageEnable;
		trait.m_independentBlendEnable = option.m_independentBlendEnable;

		constexpr int size = std::extent_v<decltype( option.m_renderTarget )>;
		for ( int i = 0; i < size; ++i )
		{
			auto& dst = trait.m_renderTarget[i];
			auto& src = option.m_renderTarget[i];

			dst.m_blendEnable = src.m_blendEnable;
			dst.m_srcBlend = src.m_srcBlend;
			dst.m_destBlend = src.m_destBlend;
			dst.m_blendOp = src.m_blendOp;
			dst.m_srcBlendAlpha = src.m_srcBlendAlpha;
			dst.m_destBlendAlpha = src.m_destBlendAlpha;
			dst.m_blendOpAlpha = src.m_blendOpAlpha;
			dst.m_renderTargetWriteMask = src.m_renderTargetWriteMask;
		}

		trait.m_sampleMask = option.m_sampleMask;

		BlendState state( trait );
		m_blendStates.emplace( option, state );
	}

	return found->second;
}

DepthStencilState AgaDelegator::FindOrCreate( const DepthStencilOption& option )
{
	auto found = m_depthStencilStates.find( option );
	if ( found == m_depthStencilStates.end( ) )
	{
		DEPTH_STENCIL_STATE_TRAIT trait = {
			option.m_depth.m_enable,
			option.m_depth.m_writeDepth ? DEPTH_WRITE_MODE::ALL : DEPTH_WRITE_MODE::ZERO,
			option.m_depth.m_depthFunc,
			option.m_stencil.m_enable,
			option.m_stencil.m_readMask,
			option.m_stencil.m_writeMask,
			option.m_stencil.m_frontFace,
			option.m_stencil.m_backFace
		};

		DepthStencilState state( trait );
		m_depthStencilStates.emplace( option, state );

		return state;
	}

	return found->second;
}

RasterizerState AgaDelegator::FindOrCreate( const RasterizerOption& option )
{
	auto found = m_rasterizerStates.find( option );
	if ( found == m_rasterizerStates.end( ) )
	{
		RASTERIZER_STATE_TRAIT trait = {
			option.m_isWireframe ? FILL_MODE::WIREFRAME : FILL_MODE::SOLID,
			option.m_cullMode,
			option.m_counterClockwise,
			option.m_depthBias,
			0.f,
			0.f,
			option.m_depthClipEnable,
			option.m_scissorEnable,
			option.m_multisampleEnalbe,
			option.m_antialiasedLineEnable
		};

		RasterizerState state( trait );
		m_rasterizerStates.emplace( option, state );

		return state;
	}
	
	return found->second;
}

SamplerState AgaDelegator::FindOrCreate( const SamplerOption& option )
{
	auto found = m_samplerStates.find( option );
	if ( found == m_samplerStates.end( ) )
	{
		SAMPLER_STATE_TRAIT trait = {
			static_cast<TEXTURE_FILTER::Type>( option.m_filter ),
			option.m_addressU,
			option.m_addressV,
			option.m_addressW,
			option.m_mipLODBias,
			1,	/*anisotropy option set later*/
			option.m_comparisonFunc,
			{ 1.f, 1.f, 1.f, 1.f },
			std::numeric_limits<float>::min(),
			std::numeric_limits<float>::max()
		};

		SamplerState state( trait );
		m_samplerStates.emplace( option, state );

		return state;
	}

	return found->second;
}

VertexLayout AgaDelegator::FindOrCreate( const VertexShader& vs, const VertexLayoutDesc& desc )
{
	auto found = m_vertexLayouts.find( desc );
	if ( found == m_vertexLayouts.end( ) )
	{
		VertexLayout vertexLayout( vs, desc );
		m_vertexLayouts.emplace( desc, vertexLayout );

		return vertexLayout;
	}

	return found->second;
}

AgaDelegator& GetAgaDelegator( )
{
	return g_agaDelegator;
}
