#include "stdafx.h"

#include "RenderView.h"

#include "common.h"
#include "ConstantBufferDefine.h"
#include "GameLogic.h"
#include "../RenderCore/CommonRenderer/IRenderer.h"

using namespace DirectX;

void CRenderView::OnDeviceRestore( CGameLogic& gameLogic )
{
	CreateDeviceDependentResource( gameLogic.GetRenderer( ) );
}

bool CRenderView::initialize( IRenderer& renderer )
{
	return CreateDeviceDependentResource( renderer );
}

void CRenderView::PushViewPort( const float x, const float y, const float width, const float height, const float zNear, const float zFar )
{
	Viewport viewport = { x, y, width, height, zNear, zFar };

	m_viewportList.push_back( viewport );
}

void CRenderView::PopViewPort( )
{
	if ( m_viewportList.size( ) > 0 )
	{
		m_viewportList.pop_back( );
	}
}

void CRenderView::PushScissorRect( const RECT & rect )
{
	m_scissorRectList.push_back( rect );
}

void CRenderView::PopScissorRect( )
{
	if ( m_scissorRectList.size( ) > 0 )
	{
		m_scissorRectList.pop_back( );
	}
}

void CRenderView::SetViewPort( IRenderer& renderer )
{
	renderer.SetViewports( m_viewportList );
}

void CRenderView::SetScissorRects( IRenderer& renderer )
{
	renderer.SetScissorRects( m_scissorRectList );
}

void CRenderView::CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_zFar = zFar;
	m_projectionMatrix = XMMatrixPerspectiveFovLH( fov, aspect, zNear, zFar );
}

void CRenderView::CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_zFar = zFar;
	m_projectionMatrix = XMMatrixPerspectiveFovRH( fov, aspect, zNear, zFar );
}

void CRenderView::UpdataView( )
{
	if ( ViewProjectionTrasform* pData = static_cast<ViewProjectionTrasform*>( m_viewConstantBuffer->LockBuffer( ) ) )
	{
		pData->m_view = XMMatrixTranspose( m_viewMatrix );
		pData->m_projection = XMMatrixTranspose( m_projectionMatrix );

		m_viewConstantBuffer->UnLockBuffer( );
		m_viewConstantBuffer->SetVSBuffer( static_cast<int>( VS_CONSTANT_BUFFER::VIEW_PROJECTION ) );
	}

	if ( GbufferInfo* pData = static_cast<GbufferInfo*>( m_gbufferConstantBuffer->LockBuffer( ) ) )
	{
		pData->m_zFar = m_zFar;

		auto mainView = m_viewportList.begin( );
		if ( mainView != m_viewportList.end( ) )
		{
			pData->m_targetWidth = mainView->m_width;
			pData->m_targetHeight = mainView->m_height;
		}

		m_gbufferConstantBuffer->UnLockBuffer( );
		m_gbufferConstantBuffer->SetPSBuffer( static_cast<int>( PS_CONSTANT_BUFFER::GBUFFER ) );
	}
}

CRenderView::CRenderView( )
{
	m_viewMatrix = XMMatrixIdentity( );
	m_projectionMatrix = XMMatrixIdentity( );
}

bool CRenderView::CreateDeviceDependentResource( IRenderer& renderer )
{
	BUFFER_TRAIT trait = { sizeof( ViewProjectionTrasform ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_TYPE::CONSTANT_BUFFER,
		0,
		nullptr,
		0,
		0 };

	m_viewConstantBuffer = renderer.CreateBuffer( trait );

	if ( m_viewConstantBuffer == nullptr )
	{
		return false;
	}

	trait.m_stride = sizeof( GbufferInfo );

	m_gbufferConstantBuffer = renderer.CreateBuffer( trait );

	if ( m_gbufferConstantBuffer == nullptr )
	{
		return false;
	}

	return true;
}
