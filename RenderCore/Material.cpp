#include "stdafx.h"
#include "common.h"
#include "CommonRenderer/IRenderer.h"
#include "Material.h"

#include <d3d11.h>
#include <type_traits>

using namespace RE_HANDLE_TYPE;

void Material::Bind( IRenderer& renderer )
{
	for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
	{
		if ( m_hShaders[i] != INVALID_HANDLE )
		{
			renderer.BindShader( static_cast<SHADER_TYPE>( i ), m_hShaders[i] );
		}
	}

	renderer.BindRasterizerState( m_hRasterizerState );
	renderer.BindDepthStencilState( m_hDepthStencilState );
	renderer.BindBlendState( m_hBlendState );

	for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
	{
		renderer.BindSamplerState( static_cast<SHADER_TYPE>( i ), 0, 1, &m_hSamplerState[i] );
	}
}

Material::Material( )
{
	for ( int i = 0; i < MAX_SHADER; ++i )
	{
		m_hShaders[i] = INVALID_HANDLE;
		m_hSamplerState[i] = INVALID_HANDLE;
	}
}