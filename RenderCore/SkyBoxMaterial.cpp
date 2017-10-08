#include "stdafx.h"
#include "IRenderer.h"
#include "IRenderState.h"
#include "SkyBoxMaterial.h"

void SkyBoxMaterial::Init( IRenderer& renderer )
{
	Material::Init( renderer );

	m_pShaders[SHADER_TYPE::VS] = renderer.SearchShaderByName( _T( "vsSkybox" ) );

	m_pShaders[SHADER_TYPE::PS] = renderer.SearchShaderByName( _T( "psSkybox" ) );

	m_pSamplerState[SHADER_TYPE::PS] = renderer.CreateSamplerState( _T( "default" ) );

	m_pDepthStencilState = renderer.CreateDepthStencilState( _T( "depthWriteOff" ) );
}

void SkyBoxMaterial::SetShader( )
{
	Material::SetShader( );
	m_pSamplerState[SHADER_TYPE::PS]->Set( SHADER_TYPE::PS );
}