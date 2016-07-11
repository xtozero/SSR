#include "stdafx.h"
#include "IRenderer.h"
#include "IRenderState.h"
#include "SkyBoxMaterial.h"

void SkyBoxMaterial::Init( IRenderer* pRenderer )
{
	Material::Init( pRenderer );

	if ( pRenderer )
	{
		m_pShaders[SHADER_TYPE::VS] = pRenderer->SearchShaderByName( _T( "vsSkybox" ) );

		m_pShaders[SHADER_TYPE::PS] = pRenderer->SearchShaderByName( _T( "psSkybox" ) );

		m_pSamplerState[SHADER_TYPE::PS] = pRenderer->CreateSamplerState( _T( "default" ) );

		m_pDepthStencilState = pRenderer->CreateDepthStencilState( _T( "depthWriteOff" ) );
	}
}

void SkyBoxMaterial::SetShader( ID3D11DeviceContext* pDeviceContext )
{
	Material::SetShader( pDeviceContext );
	m_pSamplerState[SHADER_TYPE::PS]->Set( pDeviceContext, SHADER_TYPE::PS );
}