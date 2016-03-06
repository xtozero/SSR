#include "stdafx.h"
#include "IRenderer.h"
#include "ISampler.h"
#include "SkyBoxMaterial.h"

extern IRenderer* g_pRenderer;

void SkyBoxMaterial::Init( )
{
	m_pShaders[SHADER_TYPE::VS] = g_pRenderer->SearchShaderByName( _T( "vsSkybox" ) );

	m_pShaders[SHADER_TYPE::PS] = g_pRenderer->SearchShaderByName( _T( "psSkybox" ) );

	m_pSamplerState[SHADER_TYPE::PS] = g_pRenderer->CreateSamplerState( _T( "default" ) );

	m_pDepthStencilState = g_pRenderer->CreateDepthStencilState( _T( "depthWriteOff" ) );
}

void SkyBoxMaterial::SetShader( ID3D11DeviceContext* pDeviceContext )
{
	Material::SetShader( pDeviceContext );

	if ( m_pSamplerState[SHADER_TYPE::PS] )
	{
		m_pSamplerState[SHADER_TYPE::PS]->SetSampler( pDeviceContext, SHADER_TYPE::PS );
	}
}