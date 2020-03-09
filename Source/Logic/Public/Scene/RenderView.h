#pragma once

#include "INotifyGraphicsDevice.h"
#include "Math/CXMFloat.h"
#include "Render/Resource.h"

#include <d3d11.h>
#include <vector>

class IRenderer;

class CRenderView
{
public:
	void SetViewPort( IRenderer& renderer, const Viewport* viewPorts, int count );
	void SetScissorRects( IRenderer& renderer, const RECT* rects, int count );

	void CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar );
	void CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar );

	void UpdataView( CGameLogic& gameLogic, RE_HANDLE viewProjBuffer );

	void SetViewMatrix( const CXMFLOAT4X4& viewMat ) { m_viewMatrix = viewMat; }

	const CXMFLOAT4X4& GetViewMatrix( ) const { return m_viewMatrix; }
	const CXMFLOAT4X4& GetProjectionMatrix( ) const { return m_projectionMatrix; }

	float GetNear( ) const { return m_zNear; }
	float GetFar( ) const { return m_zFar; }
	float GetAspect( ) const { return m_aspect; }
	float GetFov( ) const { return m_fov; }

	CRenderView( );

private:
	Viewport m_viewportList;
	RECT m_scissorRectList;

	CXMFLOAT4X4 m_viewMatrix;
	CXMFLOAT4X4 m_projectionMatrix;

	float m_zNear = 0.f;
	float m_zFar = 0.f;
	float m_aspect = 0.f;
	float m_fov = 0.f;
};

