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
	m_zNear = zNear;
	m_zFar = zFar;
	m_projectionMatrix = XMMatrixPerspectiveFovLH( fov, aspect, zNear, zFar );
}

void CRenderView::CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_zNear = zNear;
	m_zFar = zFar;
	m_projectionMatrix = XMMatrixPerspectiveFovRH( fov, aspect, zNear, zFar );
}

void CRenderView::UpdataView( IRenderer& renderer )
{
	if ( ViewProjectionTrasform* pData = static_cast<ViewProjectionTrasform*>( renderer.LockBuffer( m_viewConstantBuffer ) ) )
	{
		pData->m_view = XMMatrixTranspose( m_viewMatrix );
		pData->m_projection = XMMatrixTranspose( m_projectionMatrix );

		renderer.UnLockBuffer( m_viewConstantBuffer );
		renderer.BindConstantBuffer( SHADER_TYPE::VS, static_cast<int>( VS_CONSTANT_BUFFER::VIEW_PROJECTION ), 1, &m_viewConstantBuffer );
	}

	if ( PASS_CONSTANT* pData = static_cast<PASS_CONSTANT*>( renderer.LockBuffer( m_gbufferConstantBuffer ) ) )
	{
		pData->m_zFar = m_zFar;
		pData->m_zNear = m_zNear;
		pData->m_elapsedTime = CTimer::GetInstance().GetElapsedTIme();
		pData->m_totalTime = CTimer::GetInstance().GetTotalTime();

		auto mainView = m_viewportList.begin( );
		if ( mainView != m_viewportList.end( ) )
		{
			pData->m_renderTargetSize.x = mainView->m_width;
			pData->m_renderTargetSize.y = mainView->m_height;
			pData->m_invRenderTargetSize.x = 1.f / mainView->m_width;
			pData->m_invRenderTargetSize.y = 1.f / mainView->m_height;
		}

		renderer.UnLockBuffer( m_gbufferConstantBuffer );
		renderer.BindConstantBuffer( SHADER_TYPE::PS, static_cast<int>( PS_CONSTANT_BUFFER::GBUFFER ), 1, &m_gbufferConstantBuffer );
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

	if ( m_viewConstantBuffer == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	trait.m_stride = sizeof( PASS_CONSTANT );

	m_gbufferConstantBuffer = renderer.CreateBuffer( trait );

	if ( m_gbufferConstantBuffer == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	return true;
}
