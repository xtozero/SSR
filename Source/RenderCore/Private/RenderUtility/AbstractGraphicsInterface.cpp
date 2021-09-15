#include "stdafx.h"
#include "AbstractGraphicsInterface.h"

#include "Shader.h"

AbstractGraphicsInterface g_abstractGraphicsInterface;

void AbstractGraphicsInterface::BootUp( aga::IAga* pAga )
{
	m_aga = pAga;
	m_defaultConstants.BootUp( );
}

void AbstractGraphicsInterface::Shutdown( )
{
	m_defaultConstants.Shutdown( );
}

void* AbstractGraphicsInterface::Lock( aga::Buffer* buffer, uint32 lockFlag, uint32 subResource )
{
	return m_aga->Lock( buffer, lockFlag, subResource );
}

void AbstractGraphicsInterface::UnLock( aga::Buffer* buffer, uint32 subResource )
{
	m_aga->UnLock( buffer, subResource );
}

void AbstractGraphicsInterface::SetViewports( aga::Viewport** pViewPorts, uint32 size )
{
	m_aga->SetViewports( pViewPorts, size );
}

void AbstractGraphicsInterface::SetViewport( uint32 minX, uint32 minY, float minZ, uint32 maxX, uint32 maxY, float maxZ )
{
	m_aga->SetViewport( minX, minY, minZ, maxX, maxY, maxZ );
}

void AbstractGraphicsInterface::SetScissorRects( aga::Viewport** pViewPorts, uint32 size )
{
	m_aga->SetScissorRects( pViewPorts, size );
}

void AbstractGraphicsInterface::SetScissorRect( uint32 minX, uint32 minY, uint32 maxX, uint32 maxY )
{
	m_aga->SetScissorRect( minX, minY, maxX, maxY );
}

void AbstractGraphicsInterface::BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil )
{
	m_aga->BindRenderTargets( pRenderTargets, renderTargetCount, depthStencil );
}

void AbstractGraphicsInterface::ClearDepthStencil( aga::Texture* depthStencil, float depthColor, UINT8 stencilColor )
{
	m_aga->ClearDepthStencil( depthStencil, depthColor, stencilColor );
}

void AbstractGraphicsInterface::Dispatch( uint32 x, uint32 y, uint32 z )
{
	ComputeShader empty;
	m_defaultConstants.Commit( empty );

	m_aga->Dispatch( x, y, z );

	BindShader( empty );
}

void AbstractGraphicsInterface::Copy( aga::Buffer* dst, aga::Buffer* src, uint32 size )
{
	m_aga->Copy( dst, src, size );
}

BlendState AbstractGraphicsInterface::FindOrCreate( const BlendOption& option )
{
	auto found = m_blendStates.find( option );
	if ( found == m_blendStates.end( ) )
	{
		BLEND_STATE_TRAIT trait;
		trait.m_alphaToConverageEnable = option.m_alphaToConverageEnable;
		trait.m_independentBlendEnable = option.m_independentBlendEnable;

		constexpr uint32 size = std::extent_v<decltype( option.m_renderTarget )>;
		for ( uint32 i = 0; i < size; ++i )
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

		return state;
	}

	return found->second;
}

DepthStencilState AbstractGraphicsInterface::FindOrCreate( const DepthStencilOption& option )
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

RasterizerState AbstractGraphicsInterface::FindOrCreate( const RasterizerOption& option )
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

SamplerState AbstractGraphicsInterface::FindOrCreate( const SamplerOption& option )
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

VertexLayout AbstractGraphicsInterface::FindOrCreate( const VertexShader& vs, const VertexLayoutDesc& desc )
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

AbstractGraphicsInterface& GraphicsInterface( )
{
	return g_abstractGraphicsInterface;
}
