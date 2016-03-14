#include "stdafx.h"
#include "common.h"
#include "IShader.h"
#include "IShaderResource.h"
#include "Material.h"

#include <d3d11.h>

void Material::Init( )
{

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

		pDeviceContext->OMSetDepthStencilState( m_pDepthStencilState.Get( ), 1 );

		for ( int i = 0; i < SHADER_TYPE::MAX_SHADER; ++i )
		{
			if ( m_pSamplerState[i] )
			{
				m_pSamplerState[i]->SetSampler( pDeviceContext, static_cast<SHADER_TYPE>( i ) );
			}
		}
	}
}

void Material::SetTexture( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, std::shared_ptr<IShaderResource> pTexture )
{
	if ( pDeviceContext && shaderType < SHADER_TYPE::MAX_SHADER )
	{
		IShader* tagetShader = m_pShaders[shaderType].get( );

		if ( tagetShader )
		{
			if ( pTexture && !pTexture->IsTexture( ) )
			{
				//여기서 뷰의 원본이 깊이 버퍼 혹은 랜더 타겟이면 해당 뷰는 빼도록 수정 필요
				_asm
				{
					nop;
				}
			}

			tagetShader->SetShaderResource( pDeviceContext, slot, pTexture );
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
	m_pDepthStencilState( nullptr )
{
	for ( int i = 0; i < MAX_SHADER; ++i )
	{
		m_pShaders[i] = nullptr;
		m_pSamplerState[i] = nullptr;
	}
}