#include "stdafx.h"

#include "common.h"
#include "RenderView.h"
#include "ConstantBufferDefine.h"

namespace
{
	constexpr int CB_ELEMENT_NUMBER = 2;
}

bool RenderView::initialize( ID3D11Device* pDevice )
{
	ON_FAIL_RETURN( m_viewConstantBuffer.CreateBuffer( pDevice, sizeof( D3DXMATRIX ), CB_ELEMENT_NUMBER, nullptr ) );

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
	D3DXMatrixPerspectiveFovLH( &m_projectionMatrix, fov, aspect, zNear, zFar );
}

void RenderView::CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar )
{
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
}

RenderView::RenderView( )
{
	D3DXMatrixIdentity( &m_viewMatrix );
	D3DXMatrixIdentity( &m_projectionMatrix );
}


RenderView::~RenderView( )
{
}

