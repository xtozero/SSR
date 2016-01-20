#include "stdafx.h"
#include "TextureMaterial.h"

void TextureMaterial::Init( )
{
	m_pShaders[SHADER_TYPE::VS] = g_pRenderer->SearchShaderByName( _T( "vsTexture" ) );

	m_pShaders[SHADER_TYPE::PS] = g_pRenderer->SearchShaderByName( _T( "psTexture" ) );

	m_pSamplerState[SHADER_TYPE::PS] = g_pRenderer->CreateSampler( );
}

void TextureMaterial::SetShader( ID3D11DeviceContext* pDeviceContext )
{
	Material<TextureMaterial>::SetShader( pDeviceContext );

	if ( m_pSamplerState[SHADER_TYPE::PS] )
	{
		pDeviceContext->PSSetSamplers( 0, 1, m_pSamplerState[SHADER_TYPE::PS].GetAddressOf() );
	}
}

TextureMaterial::TextureMaterial( )
{
}

TextureMaterial::~TextureMaterial( )
{
}
