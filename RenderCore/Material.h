#pragma once

#include <d3d11.h>
#include "Direct3D11.h"
#include "IMaterial.h"
#include "IRenderer.h"
#include "IShader.h"
#include "ITexture.h"
#include "MaterialSystem.h"

extern IRenderer* g_pRenderer;

class IShader;

enum SHADER_TYPE
{
	VS = 0,
	HS,
	DS,
	GS,
	PS,
	CS,
	MAX_SHADER
};

template< typename T >
class Material : public IMaterial
{
protected:
	IShader* m_pShaders[SHADER_TYPE::MAX_SHADER];
	ID3D11RasterizerState* m_pRenderState;
	ID3D11SamplerState* m_pSamplerState[SHADER_TYPE::MAX_SHADER];

public:
	virtual void Init( ) override;
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) override;
	virtual void SetTexture( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, std::shared_ptr<ITexture> pTexture ) override;

	virtual void Draw( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT vertexOffset = 0 ) override;
	virtual void DrawIndexed( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT indexOffset = 0, const UINT vertexOffset = 0 ) override;
	void DrawInstanced( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT instanceCount, const UINT vertexOffset = 0, const UINT instanceOffset = 0 );
	void DrawInstancedInstanced( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT instanceCount, const UINT indexOffset = 0, const UINT vertexOffset = 0, const UINT instanceOffset = 0 );
	void DrawAuto( ID3D11DeviceContext* pDeviceContext );

	void SetPrimitiveTopology( ID3D11DeviceContext* pDeviceContext, D3D11_PRIMITIVE_TOPOLOGY primtopology );

	Material( );
	virtual ~Material( );
};

template< typename T >
void Material<T>::Init( )
{

}

template< typename T >
void Material<T>::SetShader( ID3D11DeviceContext* pDeviceContext )
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

		pDeviceContext->RSSetState( m_pRenderState );
	}
}

template< typename T >
void Material<T>::SetTexture( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, std::shared_ptr<ITexture> pTexture )
{
	if ( pDeviceContext && shaderType < SHADER_TYPE::MAX_SHADER )
	{
		IShader* tagetShader = m_pShaders[shaderType];
		ID3D11ShaderResourceView* pResourceView = nullptr;

		if ( pTexture )
		{
			pResourceView = pTexture->GetResource( );
		}

		if ( tagetShader )
		{
			tagetShader->SetShaderResource( pDeviceContext, slot, pResourceView );
		}
	}
}

template< typename T >
void Material<T>::Draw( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT vertexOffset )
{
	if ( pDeviceContext )
	{
		pDeviceContext->Draw( vertexCount, vertexOffset );
	}
}

template< typename T >
void Material<T>::DrawIndexed( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT indexOffset, const UINT vertexOffset )
{
	if ( pDeviceContext )
	{
		pDeviceContext->DrawIndexed( indexCount, indexOffset, vertexOffset );
	}
}

template< typename T >
void Material<T>::DrawInstanced( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT instanceCount, const UINT vertexOffset, const UINT instanceOffset )
{
	if ( pDeviceContext )
	{
		pDeviceContext->DrawInstanced( vertexCount, instanceCount, vertexOffset, instanceOffset );
	}
}

template< typename T >
void Material<T>::DrawInstancedInstanced( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT instanceCount, const UINT indexOffset, const UINT vertexOffset, const UINT instanceOffset )
{
	if ( pDeviceContext )
	{
		pDeviceContext->DrawIndexedInstanced( indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset );
	}
}

template< typename T >
void Material<T>::DrawAuto( ID3D11DeviceContext* pDeviceContext )
{
	if ( pDeviceContext )
	{
		pDeviceContext->DrawAuto( );
	}
}

template< typename T >
void Material<T>::SetPrimitiveTopology( ID3D11DeviceContext* pDeviceContext, D3D11_PRIMITIVE_TOPOLOGY primtopology )
{
	if ( pDeviceContext )
	{
		pDeviceContext->IASetPrimitiveTopology( primtopology );
	}
}

template< typename T >
Material<T>::Material( ) :
m_pRenderState( nullptr )
{
	for ( int i = 0; i < MAX_SHADER; ++i )
	{
		m_pShaders[i] = nullptr;
		m_pSamplerState[i] = nullptr;
	}
}

template< typename T >
Material<T>::~Material( )
{
	//Do not delete m_pShader's pointer variable in here
	SAFE_RELEASE( m_pRenderState );
	for ( int i = 0; i < MAX_SHADER; ++i )
	{
		SAFE_RELEASE( m_pSamplerState[i] );
	}
}