#include "stdafx.h"
#include "common.h"
#include "CommonRenderer/IBuffer.h"
#include "CommonRenderer/IRenderer.h"
#include "CommonRenderer/IRenderState.h"
#include "CommonRenderer/IRenderResource.h"
#include "CommonRenderer/IShader.h"
#include "Material.h"

#include <d3d11.h>
#include <type_traits>

void Material::Init( IRenderer& renderer )
{
	m_pRasterizerState = renderer.CreateRenderState( _T( "NULL" ) );
	m_pDepthStencilState = renderer.CreateDepthStencilState( _T( "NULL" ) );
	for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
	{
		m_pSamplerState[i] = renderer.CreateSamplerState( _T( "NULL" ) );
	}
	m_pBlendState = renderer.CreateBlendState( _T( "NULL" ) );
}

void Material::SetShader( )
{
	for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
	{
		if ( m_pShaders[i] )
		{
			m_pShaders[i]->SetShader( );
		}
	}

	m_pRasterizerState->Set( );
	m_pDepthStencilState->Set( );
	m_pBlendState->Set( );

	for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
	{
		m_pSamplerState[i]->Set( static_cast<SHADER_TYPE>( i ) );
	}
}

void Material::SetTexture( UINT shaderType, UINT slot, const IRenderResource* pTexture )
{
	if ( shaderType < SHADER_TYPE::MAX_SHADER )
	{
		if ( m_pShaders[shaderType] )
		{
			m_pShaders[shaderType]->SetShaderResource( slot, pTexture );
		}
	}
}

Material::Material( ) :
	m_pRasterizerState( nullptr ),
	m_pDepthStencilState( nullptr ),
	m_pBlendState( nullptr )
{
	for ( int i = 0; i < MAX_SHADER; ++i )
	{
		m_pShaders[i] = nullptr;
		m_pSamplerState[i] = nullptr;
	}
}