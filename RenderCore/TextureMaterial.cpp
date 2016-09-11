#include "stdafx.h"
#include "IRenderer.h"
#include "IRenderState.h"
#include "TextureMaterial.h"

void TextureMaterial::Init( IRenderer& renderer )
{
	Material::Init( renderer );

	m_pShaders[SHADER_TYPE::VS] = renderer.SearchShaderByName( _T( "vsTexture" ) );

	m_pShaders[SHADER_TYPE::PS] = renderer.SearchShaderByName( _T( "psTexture" ) );

	m_pSamplerState[SHADER_TYPE::PS] = renderer.CreateSamplerState( _T( "default" ) );
}

void TextureMaterial::SetShader( ID3D11DeviceContext* pDeviceContext )
{
	Material::SetShader( pDeviceContext );
	m_pSamplerState[SHADER_TYPE::PS]->Set( pDeviceContext, SHADER_TYPE::PS );
}
