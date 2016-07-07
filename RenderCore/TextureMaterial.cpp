#include "stdafx.h"
#include "IRenderer.h"
#include "ISampler.h"
#include "TextureMaterial.h"

void TextureMaterial::Init( IRenderer* pRenderer )
{
	Material::Init( pRenderer );

	if ( pRenderer )
	{
		m_pShaders[SHADER_TYPE::VS] = pRenderer->SearchShaderByName( _T( "vsTexture" ) );

		m_pShaders[SHADER_TYPE::PS] = pRenderer->SearchShaderByName( _T( "psTexture" ) );

		m_pSamplerState[SHADER_TYPE::PS] = pRenderer->CreateSamplerState( _T( "default" ) );
	}
}

void TextureMaterial::SetShader( ID3D11DeviceContext* pDeviceContext )
{
	Material::SetShader( pDeviceContext );

	if ( m_pSamplerState[SHADER_TYPE::PS] )
	{
		m_pSamplerState[SHADER_TYPE::PS]->SetSampler( pDeviceContext, SHADER_TYPE::PS );
	}
}
