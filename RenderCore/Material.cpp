#include "stdafx.h"
#include "common.h"
#include "IBuffer.h"
#include "IRenderer.h"
#include "IRenderState.h"
#include "IShader.h"
#include "IRenderResource.h"
#include "ISurface.h"
#include "Material.h"

#include <d3d11.h>
#include <type_traits>

void Material::Init( IRenderer& renderer )
{
	m_pRasterizerState = renderer.CreateRenderState( _T( "NULL" ) );
	m_pDepthStencilState = renderer.CreateDepthStencilState( _T( "NULL" ) );
	for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
	{
		m_pSamplerState[i] = renderer.CreateSamplerState( _T( "NULL" ) );
	}
	m_pBlendState = renderer.CreateBlendState( _T( "NULL" ) );
}

void Material::SetShader( )
{
	for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
	{
		if ( m_pShaders[i] )
		{
			m_pShaders[i]->SetShader( );
		}
	}

	m_pRasterizerState->Set( );
	m_pDepthStencilState->Set( );
	m_pBlendState->Set( );

	for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
	{
		m_pSamplerState[i]->Set( static_cast<SHADER_TYPE>( i ) );
	}
}

void Material::SetTexture( UINT shaderType, UINT slot, const IRenderResource* pTexture )
{
	if ( shaderType < SHADER_TYPE::MAX_SHADER )
	{
		if ( m_pShaders[shaderType] )
		{
			m_pShaders[shaderType]->SetShaderResource( slot, pTexture );
		}
	}
}

void Material::SetSurface( UINT shaderType, UINT slot, const ISurface* pSurface )
{
 	if ( m_pConstantBuffers && pSurface && shaderType < SHADER_TYPE::MAX_SHADER )
	{
		IBuffer* pSurfaceBuffer = (*m_pConstantBuffers)[MAT_CONSTANT_BUFFER::SURFACE];

		if ( m_pShaders[shaderType] && pSurfaceBuffer )
		{
			const SurfaceTrait& src = pSurface->GetTrait( );
			void* dest = pSurfaceBuffer->LockBuffer( );

			if ( dest )
			{
				::memcpy_s( dest, pSurfaceBuffer->Size( ), &src, sizeof( SurfaceTrait ) );
				pSurfaceBuffer->UnLockBuffer( );
				m_pShaders[shaderType]->SetConstantBuffer( slot, pSurfaceBuffer );
			}
			else
			{
				pSurfaceBuffer->UnLockBuffer( );
			}
		}
	}
}

Material::Material( ) :
	m_pRasterizerState( nullptr ),
	m_pDepthStencilState( nullptr ),
	m_pConstantBuffers( nullptr ),
	m_pBlendState( nullptr )
{
	for ( int i = 0; i < MAX_SHADER; ++i )
	{
		m_pShaders[i] = nullptr;
		m_pSamplerState[i] = nullptr;
	}
}