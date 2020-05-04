#include "stdafx.h"
#include "Material.h"

#include "common.h"

#include <type_traits>

void CMaterial::Bind( IRenderer& renderer ) const
{
	for ( int i = 0; i < MAX_SHADER_TYPE<int>; ++i )
	{
		// renderer.BindShader( static_cast<SHADER_TYPE>( i ), m_hShaders[i] );
	}

	// renderer.BindRasterizerState( m_hRasterizerState );
	// renderer.BindDepthStencilState( m_hDepthStencilState );
	// renderer.BindBlendState( m_hBlendState );

	for ( int i = 0; i < MAX_SHADER_TYPE<int>; ++i )
	{
		// renderer.BindSamplerState( static_cast<SHADER_TYPE>( i ), 0, 1, &m_hSamplerState[i] );
	}
}
