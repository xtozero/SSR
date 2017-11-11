#include "stdafx.h"
#include "CommonRenderer/IRenderer.h"
#include "CommonRenderer/IRenderState.h"
#include "TextureMaterial.h"

void TextureMaterial::Init( IRenderer& renderer )
{
	Material::Init( renderer );

	m_pShaders[SHADER_TYPE::VS] = renderer.FindGraphicsShaderByName( _T( "vsTexture" ) );

	m_pShaders[SHADER_TYPE::PS] = renderer.FindGraphicsShaderByName( _T( "psTexture" ) );

	m_pSamplerState[SHADER_TYPE::PS] = renderer.CreateSamplerState( _T( "default" ) );
}

void TextureMaterial::SetShader( )
{
	Material::SetShader( );
	m_pSamplerState[SHADER_TYPE::PS]->Set( SHADER_TYPE::PS );
}
