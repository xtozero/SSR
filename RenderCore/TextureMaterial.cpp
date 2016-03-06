#include "stdafx.h"
#include "IRenderer.h"
#include "ISampler.h"
#include "TextureMaterial.h"

extern IRenderer* g_pRenderer;

void TextureMaterial::Init( )
{
	m_pShaders[SHADER_TYPE::VS] = g_pRenderer->SearchShaderByName( _T( "vsTexture" ) );

	m_pShaders[SHADER_TYPE::PS] = g_pRenderer->SearchShaderByName( _T( "psTexture" ) );

	m_pSamplerState[SHADER_TYPE::PS] = g_pRenderer->CreateSamplerState( _T( "default" ) );
}

void TextureMaterial::SetShader( ID3D11DeviceContext* pDeviceContext )
{
	Material::SetShader( pDeviceContext );

	if ( m_pSamplerState[SHADER_TYPE::PS] )
	{
		m_pSamplerState[SHADER_TYPE::PS]->SetSampler( pDeviceContext, SHADER_TYPE::PS );
	}
}
