#include "stdafx.h"
#include "CommonRenderer/IRenderer.h"
#include "CommonRenderer/IRenderState.h"
#include "SkyBoxMaterial.h"

void SkyBoxMaterial::Init( IRenderer& renderer )
{
	Material::Init( renderer );

	m_pShaders[SHADER_TYPE::VS] = renderer.FindGraphicsShaderByName( _T( "vsSkybox" ) );

	m_pShaders[SHADER_TYPE::PS] = renderer.FindGraphicsShaderByName( _T( "psSkybox" ) );

	m_pSamplerState[SHADER_TYPE::PS] = renderer.CreateSamplerState( _T( "default" ) );

	m_pDepthStencilState = renderer.CreateDepthStencilState( _T( "depthWriteOff" ) );
}

void SkyBoxMaterial::SetShader( )
{
	Material::SetShader( );
	m_pSamplerState[SHADER_TYPE::PS]->Set( SHADER_TYPE::PS );
}