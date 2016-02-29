#include "stdafx.h"
#include "SkyBoxMaterial.h"

void SkyBoxMaterial::Init( )
{
	m_pShaders[SHADER_TYPE::VS] = g_pRenderer->SearchShaderByName( _T( "vsSkybox" ) );

	m_pShaders[SHADER_TYPE::PS] = g_pRenderer->SearchShaderByName( _T( "psSkybox" ) );

	m_pSamplerState[SHADER_TYPE::PS] = g_pRenderer->CreateSamplerState( _T( "default" ) );

	m_pDepthStencilState = g_pRenderer->CreateDepthStencilState( _T( "depthWriteOff" ) );
}

void SkyBoxMaterial::SetShader( ID3D11DeviceContext* pDeviceContext )
{
	Material<SkyBoxMaterial>::SetShader( pDeviceContext );

	if ( m_pSamplerState[SHADER_TYPE::PS] )
	{
		pDeviceContext->PSSetSamplers( 0, 1, m_pSamplerState[SHADER_TYPE::PS].GetAddressOf( ) );
	}
}