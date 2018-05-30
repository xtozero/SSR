#include "stdafx.h"
#include "Scene/RenderView.h"

#include "common.h"
#include "Core/GameLogic.h"
#include "Render/IRenderer.h"
#include "Scene/ConstantBufferDefine.h"

using namespace DirectX;

void CRenderView::SetViewPort( IRenderer& renderer, const Viewport* viewPorts, int count )
{
	renderer.SetViewports( viewPorts, count );
}

void CRenderView::SetScissorRects( IRenderer& renderer, const RECT* rects, int count )
{
	renderer.SetScissorRects( rects, count );
}

void CRenderView::CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_fov = fov;
	m_aspect = aspect;
	m_zNear = zNear;
	m_zFar = zFar;
	m_projectionMatrix = XMMatrixPerspectiveFovLH( fov, aspect, zNear, zFar );
}

void CRenderView::CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar )
{
	m_fov = fov;
	m_aspect = aspect;
	m_zNear = zNear;
	m_zFar = zFar;
	m_projectionMatrix = XMMatrixPerspectiveFovRH( fov, aspect, zNear, zFar );
}

void CRenderView::UpdataView( CGameLogic& gameLogic, RE_HANDLE viewProjBuffer )
{
	IRenderer& renderer = gameLogic.GetRenderer( );

	if ( ViewProjectionTrasform* pData = static_cast<ViewProjectionTrasform*>( renderer.LockBuffer( viewProjBuffer ) ) )
	{
		pData->m_view = XMMatrixTranspose( m_viewMatrix );
		pData->m_projection = XMMatrixTranspose( m_projectionMatrix );

		CShadowManager& shadowManager = gameLogic.GetShadowManager( );
		pData->m_lightViewProjection = XMMatrixTranspose( shadowManager.GetLightViewProjectionMatrix( ) );

		renderer.UnLockBuffer( viewProjBuffer );
		renderer.BindConstantBuffer( SHADER_TYPE::VS, static_cast<int>( VS_CONSTANT_BUFFER::VIEW_PROJECTION ), 1, &viewProjBuffer );
	}
}

CRenderView::CRenderView( )
{
	m_viewMatrix = XMMatrixIdentity( );
	m_projectionMatrix = XMMatrixIdentity( );
}
