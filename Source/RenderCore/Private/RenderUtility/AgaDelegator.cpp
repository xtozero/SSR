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

		auto state = DepthStencilState::Create( trait );
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

		auto state = RasterizerState::Create( trait );
		m_rasterizerStates.emplace( option, state );

		return state;
	}
	
	return found->second;
}

AgaDelegator& GetAgaDelegator( )
{
	return g_agaDelegator;
}
