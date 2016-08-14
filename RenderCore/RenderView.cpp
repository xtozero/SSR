#include "stdafx.h"

#include "common.h"
#include "RenderView.h"
#include "ConstantBufferDefine.h"

namespace
{
	constexpr int CB_ELEMENT_NUMBER = 2;

	struct VIEW_PROJECTION
	{
		D3DXMATRIX m_view;
		D3DXMATRIX m_projection;
	};

	struct GBUFFER_INFO
	{
		float m_zFar;
		float m_padding[3];
	};
}

bool RenderView::initialize( ID3D11Device* pDevice )
{
	ON_FAIL_RETURN( m_viewConstantBuffer.CreateBuffer( pDevice, sizeof( D3DXMATRIX ), CB_ELEMENT_NUMBER, nullptr ) );
	ON_FAIL_RETURN( m_gBufferConstantBuffer.CreateBuffer( pDevice, sizeof( GBUFFER_INFO ), 1, nullptr ) );

	return true;
}

void RenderView::PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth, const float maxDepth )
{
	D3D11_VIEWPORT viewport = { topLeftX, topLeftY, width, height, minDepth, maxDepth };

	m_viewportList.push_back( viewport );
}

void RenderView::PopViewPort( )
{
	if ( m_viewportList.size( ) > 0 )
	{
		m_viewportList.pop_back( );
	}
}

void RenderView::SetViewPort( ID3D11DeviceContext* pDeviceContext )
{
	pDeviceContext->RSSetViewports( m_viewportList.size( ), m_viewportList.begin( )._Ptr );
}

void RenderView::CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_zFar = zFar;
	D3DXMatrixPerspectiveFovLH( &m_projectionMatrix, fov, aspect, zNear, zFar );
}

void RenderView::CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_zFar = zFar;
	D3DXMatrixPerspectiveFovRH( &m_projectionMatrix, fov, aspect, zNear, zFar );
}

void RenderView::UpdataView( ID3D11DeviceContext* pDeviceContext )
{
	if ( VIEW_PROJECTION* pData = static_cast<VIEW_PROJECTION*>( m_viewConstantBuffer.LockBuffer( pDeviceContext ) ) )
	{
		D3DXMatrixTranspose( &pData->m_view, &m_viewMatrix );
		D3DXMatrixTranspose( &pData->m_projection, &m_projectionMatrix );

		m_viewConstantBuffer.UnLockBuffer( pDeviceContext );
		m_viewConstantBuffer.SetVSBuffer( pDeviceContext, static_cast<int>( VS_CONSTANT_BUFFER::VIEW_PROJECTION ) );
	}

	if ( GBUFFER_INFO* pData = static_cast<GBUFFER_INFO*>( m_gBufferConstantBuffer.LockBuffer( pDeviceContext ) ) )
	{
		pData->m_zFar = m_zFar;

		m_gBufferConstantBuffer.UnLockBuffer( pDeviceContext );
		m_gBufferConstantBuffer.SetPSBuffer( pDeviceContext, static_cast<int>(PS_CONSTANT_BUFFER::GBUFFER) );
	}
}

RenderView::RenderView( ) : 
	m_zFar( 0.f )
{
	D3DXMatrixIdentity( &m_viewMatrix );
	D3DXMatrixIdentity( &m_projectionMatrix );
}


RenderView::~RenderView( )
{
}

