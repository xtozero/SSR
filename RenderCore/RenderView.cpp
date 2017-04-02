#include "stdafx.h"

#include "common.h"
#include "RenderView.h"
#include "ConstantBufferDefine.h"

using namespace DirectX;

namespace
{
	constexpr int CB_ELEMENT_NUMBER = 2;

	struct VIEW_PROJECTION
	{
		CXMFLOAT4X4 m_view;
		CXMFLOAT4X4 m_projection;
	};

	struct GBUFFER_INFO
	{
		float m_zFar;
		float m_targetWidth;
		float m_targetHeight;
		float m_padding;
	};
}

bool RenderView::initialize( ID3D11Device* pDevice )
{
	ON_FAIL_RETURN( m_viewConstantBuffer.CreateBuffer( pDevice, sizeof( CXMFLOAT4X4 ), CB_ELEMENT_NUMBER, nullptr ) );
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

void RenderView::PushScissorRect( const RECT & rect )
{
	m_scissorRectList.push_back( rect );
}

void RenderView::PopScissorRect( )
{
	if ( m_scissorRectList.size( ) > 0 )
	{
		m_scissorRectList.pop_back( );
	}
}

void RenderView::SetViewPort( ID3D11DeviceContext* pDeviceContext )
{
	pDeviceContext->RSSetViewports( m_viewportList.size( ), m_viewportList.data( ) );
}

void RenderView::SetScissorRects( ID3D11DeviceContext * pDeviceContext )
{
	pDeviceContext->RSSetScissorRects( m_scissorRectList.size( ), m_scissorRectList.data( ) );
}

void RenderView::CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_zFar = zFar;
	m_projectionMatrix = XMMatrixPerspectiveFovLH( fov, aspect, zNear, zFar );
}

void RenderView::CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_zFar = zFar;
	m_projectionMatrix = XMMatrixPerspectiveFovRH( fov, aspect, zNear, zFar );
}

void RenderView::UpdataView( ID3D11DeviceContext* pDeviceContext )
{
	if ( VIEW_PROJECTION* pData = static_cast<VIEW_PROJECTION*>( m_viewConstantBuffer.LockBuffer( pDeviceContext ) ) )
	{
		pData->m_view = XMMatrixTranspose( m_viewMatrix );
		pData->m_projection = XMMatrixTranspose( m_projectionMatrix );

		m_viewConstantBuffer.UnLockBuffer( pDeviceContext );
		m_viewConstantBuffer.SetVSBuffer( pDeviceContext, static_cast<int>( VS_CONSTANT_BUFFER::VIEW_PROJECTION ) );
	}

	if ( GBUFFER_INFO* pData = static_cast<GBUFFER_INFO*>( m_gBufferConstantBuffer.LockBuffer( pDeviceContext ) ) )
	{
		pData->m_zFar = m_zFar;

		auto mainView = m_viewportList.begin( );
		if ( mainView != m_viewportList.end( ) )
		{
			pData->m_targetWidth = mainView->Width;
			pData->m_targetHeight = mainView->Height;
		}

		m_gBufferConstantBuffer.UnLockBuffer( pDeviceContext );
		m_gBufferConstantBuffer.SetPSBuffer( pDeviceContext, static_cast<int>(PS_CONSTANT_BUFFER::GBUFFER) );
	}
}

RenderView::RenderView( ) : 
	m_zFar( 0.f )
{
	m_viewMatrix = XMMatrixIdentity( );
	m_projectionMatrix = XMMatrixIdentity( );
}


RenderView::~RenderView( )
{
}

