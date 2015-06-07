#include "stdafx.h"
#include "common.h"

#include "Direct3D11.h"
#include "IShader.h"
#include "Material.h"

void Material::Init( )
{

}

void Material::SetShader( ID3D11DeviceContext* pDeviceContext )
{
	if ( pDeviceContext )
	{
		for ( int i = 0; i < MAX_SHADER; ++i )
		{
			if ( m_pShaders[i] )
			{
				m_pShaders[i]->SetShader( pDeviceContext );
			}
		}
	}
}

Material::Material( )
{
	for ( int i = 0; i < MAX_SHADER; ++i )
	{
		m_pShaders[i] = NULL;
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

Material::~Material( )
{
	//Do not delete m_pShader's pointer variable in here
}
