#include "stdafx.h"
#include "common.h"
#include "IBuffer.h"
#include "IRenderer.h"
#include "IRenderState.h"
#include "IShader.h"
#include "IShaderResource.h"
#include "ISurface.h"
#include "Material.h"

#include <d3d11.h>
#include <type_traits>

void Material::Init( IRenderer* pRenderer )
{
	m_pDepthStencilState = pRenderer->CreateDepthStencilState( _T( "NULL" ) );
}

void Material::SetShader( ID3D11DeviceContext* pDeviceContext )
{
	if ( pDeviceContext )
	{
		for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
		{
			if ( m_pShaders[i] )
			{
				m_pShaders[i]->SetShader( pDeviceContext );
			}
		}

		pDeviceContext->RSSetState( m_pRenderState.Get( ) );
		m_pDepthStencilState->Set( pDeviceContext );

		for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
		{
			if ( m_pSamplerState[i] )
			{
				m_pSamplerState[i]->SetSampler( pDeviceContext, static_cast<SHADER_TYPE>( i ) );
			}
		}
	}
}

void Material::SetTexture( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, const IShaderResource* pTexture )
{
	if ( pDeviceContext && shaderType < SHADER_TYPE::MAX_SHADER )
	{
		IShader* tagetShader = m_pShaders[shaderType].get( );

		if ( tagetShader )
		{
			tagetShader->SetShaderResource( pDeviceContext, slot, pTexture );
		}
	}
}

void Material::SetSurface( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, const ISurface* pSurface )
{
 	if ( pDeviceContext && m_pConstantBuffers && pSurface && shaderType < SHADER_TYPE::MAX_SHADER )
	{
		IShader* targetShader = m_pShaders[shaderType].get( );
		IBuffer* pSurfaceBuffer = m_pConstantBuffers->at( MAT_CONSTANT_BUFFER::SURFACE ).get();

		if ( targetShader && pSurfaceBuffer )
		{
			const SurfaceTrait* src = pSurface->GetTrait( );
			static_assert(std::is_trivially_copyable<decltype(src)>::value || std::is_trivial<decltype(src)>::value,
				"memcpy src class must be standard layout or trivial");

			void* dest = pSurfaceBuffer->LockBuffer( pDeviceContext );

			if ( src && dest )
			{
				::memcpy_s( dest, pSurfaceBuffer->Size(), src, sizeof( SurfaceTrait ) );
				pSurfaceBuffer->UnLockBuffer( pDeviceContext );
				targetShader->SetConstantBuffer( pDeviceContext, slot, pSurfaceBuffer );
			}
			else
			{
				pSurfaceBuffer->UnLockBuffer( pDeviceContext );
			}
		}
	}
}


void Material::Draw( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT vertexOffset )
{
	if ( pDeviceContext )
	{
		pDeviceContext->Draw( vertexCount, vertexOffset );
	}
}

void Material::DrawIndexed( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT indexOffset, const UINT vertexOffset )
{
	if ( pDeviceContext )
	{
		pDeviceContext->DrawIndexed( indexCount, indexOffset, vertexOffset );
	}
}

void Material::DrawInstanced( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT instanceCount, const UINT vertexOffset, const UINT instanceOffset )
{
	if ( pDeviceContext )
	{
		pDeviceContext->DrawInstanced( vertexCount, instanceCount, vertexOffset, instanceOffset );
	}
}

void Material::DrawInstancedInstanced( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT instanceCount, const UINT indexOffset, const UINT vertexOffset, const UINT instanceOffset )
{
	if ( pDeviceContext )
	{
		pDeviceContext->DrawIndexedInstanced( indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset );
	}
}

void Material::DrawAuto( ID3D11DeviceContext* pDeviceContext )
{
	if ( pDeviceContext )
	{
		pDeviceContext->DrawAuto( );
	}
}

void Material::SetPrimitiveTopology( ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY primtopology )
{
	if ( pDeviceContext )
	{
		pDeviceContext->IASetPrimitiveTopology( primtopology );
	}
}

Material::Material( ) :
	m_pRenderState( nullptr ),
	m_pDepthStencilState( nullptr ),
	m_pConstantBuffers( nullptr )
{
	for ( int i = 0; i < MAX_SHADER; ++i )
	{
		m_pShaders[i] = nullptr;
		m_pSamplerState[i] = nullptr;
	}
}