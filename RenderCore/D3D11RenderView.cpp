#include "stdafx.h"

#include "D3D11RenderView.h"

#include "common.h"
#include "ConstantBufferDefine.h"
#include "IRenderer.h"

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

bool D3D11RenderView::initialize( IRenderer& renderer )
{
	BUFFER_TRAIT trait = { sizeof( CXMFLOAT4X4 ),
							CB_ELEMENT_NUMBER,
							BUFFER_ACCESS_FLAG::GPU_READ | BUFFER_ACCESS_FLAG::CPU_WRITE,
							BUFFER_TYPE::CONSTANT_BUFFER,
							0,
							nullptr,
							0,
							0 };

	m_viewConstantBuffer = renderer.CreateBuffer( trait );

	if ( m_viewConstantBuffer == nullptr )
	{
		return false;
	}

	trait.m_stride = sizeof( GBUFFER_INFO );
	trait.m_count = 1;

	m_gBufferConstantBuffer = renderer.CreateBuffer( trait );

	if ( m_gBufferConstantBuffer == nullptr )
	{
		return false;
	}

	return true;
}

void D3D11RenderView::PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth, const float maxDepth )
{
	D3D11_VIEWPORT viewport = { topLeftX, topLeftY, width, height, minDepth, maxDepth };

	m_viewportList.push_back( viewport );
}

void D3D11RenderView::PopViewPort( )
{
	if ( m_viewportList.size( ) > 0 )
	{
		m_viewportList.pop_back( );
	}
}

void D3D11RenderView::PushScissorRect( const RECT & rect )
{
	m_scissorRectList.push_back( rect );
}

void D3D11RenderView::PopScissorRect( )
{
	if ( m_scissorRectList.size( ) > 0 )
	{
		m_scissorRectList.pop_back( );
	}
}

void D3D11RenderView::SetViewPort( )
{
	m_pDeviceContext->RSSetViewports( m_viewportList.size( ), m_viewportList.data( ) );
}

void D3D11RenderView::SetScissorRects( )
{
	m_pDeviceContext->RSSetScissorRects( m_scissorRectList.size( ), m_scissorRectList.data( ) );
}

void D3D11RenderView::CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_zFar = zFar;
	m_projectionMatrix = XMMatrixPerspectiveFovLH( fov, aspect, zNear, zFar );
}

void D3D11RenderView::CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_zFar = zFar;
	m_projectionMatrix = XMMatrixPerspectiveFovRH( fov, aspect, zNear, zFar );
}

void D3D11RenderView::UpdataView( )
{
	if ( VIEW_PROJECTION* pData = static_cast<VIEW_PROJECTION*>( m_viewConstantBuffer->LockBuffer( ) ) )
	{
		pData->m_view = XMMatrixTranspose( m_viewMatrix );
		pData->m_projection = XMMatrixTranspose( m_projectionMatrix );

		m_viewConstantBuffer->UnLockBuffer( );
		m_viewConstantBuffer->SetVSBuffer( static_cast<int>( VS_CONSTANT_BUFFER::VIEW_PROJECTION ) );
	}

	if ( GBUFFER_INFO* pData = static_cast<GBUFFER_INFO*>( m_gBufferConstantBuffer->LockBuffer( ) ) )
	{
		pData->m_zFar = m_zFar;

		auto mainView = m_viewportList.begin( );
		if ( mainView != m_viewportList.end( ) )
		{
			pData->m_targetWidth = mainView->Width;
			pData->m_targetHeight = mainView->Height;
		}

		m_gBufferConstantBuffer->UnLockBuffer( );
		m_gBufferConstantBuffer->SetPSBuffer( static_cast<int>(PS_CONSTANT_BUFFER::GBUFFER) );
	}
}

D3D11RenderView::D3D11RenderView( ID3D11DeviceContext* pDeviceContext ) : 
	m_pDeviceContext( pDeviceContext ),
	m_zFar( 0.f )
{
	assert( m_pDeviceContext );
	m_viewMatrix = XMMatrixIdentity( );
	m_projectionMatrix = XMMatrixIdentity( );
}


D3D11RenderView::~D3D11RenderView( )
{
}

