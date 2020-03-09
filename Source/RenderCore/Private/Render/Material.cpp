#include "stdafx.h"
#include "Render/Material.h"

#include "common.h"
#include "Render/IRenderer.h"

#include <d3d11.h>
#include <type_traits>

void CMaterial::Bind( IRenderer& renderer ) const
{
	for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
	{
		renderer.BindShader( static_cast<SHADER_TYPE>( i ), m_hShaders[i] );
	}

	renderer.BindRasterizerState( m_hRasterizerState );
	renderer.BindDepthStencilState( m_hDepthStencilState );
	renderer.BindBlendState( m_hBlendState );

	for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
	{
		renderer.BindSamplerState( static_cast<SHADER_TYPE>( i ), 0, 1, &m_hSamplerState[i] );
	}
}

CMaterial::CMaterial( )
{
	for ( int i = 0; i < MAX_SHADER; ++i )
	{
		m_hShaders[i] = RE_HANDLE::InValidHandle( );
		m_hSamplerState[i] = RE_HANDLE::InValidHandle( );
	}
}

bool CMaterial::SetShader( IRenderer& renderer, const SHADER_TYPE type, const String& shaderName )
{
	if ( type >= SHADER_TYPE::MAX_SHADER || type < SHADER_TYPE::VS )
	{
		return false;
	}

	m_hShaders[type] = renderer.FindGraphicsShaderByName( shaderName.c_str( ) );
	return ( m_hShaders[type] != RE_HANDLE::InValidHandle( ) ) ? true : false;
}

bool CMaterial::SetSamplerState( IRenderer& renderer, const SHADER_TYPE type, const String& samplerName )
{
	if ( type >= SHADER_TYPE::MAX_SHADER || type < SHADER_TYPE::VS )
	{
		return false;
	}

	m_hSamplerState[type] = renderer.CreateSamplerState( samplerName );
	return ( m_hSamplerState[type] != RE_HANDLE::InValidHandle( ) ) ? true : false;
}

bool CMaterial::SetDepthStencilState( IRenderer& renderer, const String& dsStateName )
{
	m_hDepthStencilState = renderer.CreateDepthStencilState( dsStateName );
	return ( m_hDepthStencilState != RE_HANDLE::InValidHandle( ) ) ? true : false;
}

bool CMaterial::SetRasterizerState( IRenderer& renderer, const String& rsStateName )
{
	m_hRasterizerState = renderer.CreateRasterizerState( rsStateName );
	return ( m_hRasterizerState != RE_HANDLE::InValidHandle( ) ) ? true : false;
}

bool CMaterial::SetBlendState( IRenderer& renderer, const String& blendStateName )
{
	m_hBlendState = renderer.CreateBlendState( blendStateName );
	return ( m_hBlendState != RE_HANDLE::InValidHandle( ) ) ? true : false;
}
