#pragma once

#include "INotifyGraphicsDevice.h"
#include "Math/CXMFloat.h"
#include "Render/Resource.h"

#include <d3d11.h>
#include <vector>

class IRenderer;

class CRenderView : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	bool initialize( IRenderer& renderer );

	void PushViewPort( const float x, const float y, const float width, const float height, const float zNear = 0.f, const float zFar = 1.0f );
	void PopViewPort( );
	void PushScissorRect( const RECT& rect );
	void PopScissorRect( );

	void SetViewPort( IRenderer& renderer );
	void SetScissorRects( IRenderer& renderer );

	void CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar );
	void CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar );

	void UpdataView( IRenderer& renderer, RE_HANDLE viewProjBuffer );

	void SetViewMatrix( const CXMFLOAT4X4& viewMat ) { m_viewMatrix = viewMat; }

	const CXMFLOAT4X4& GetViewMatrix( ) const { return m_viewMatrix; }
	const CXMFLOAT4X4& GetProjectionMatrix( ) const { return m_projectionMatrix; }

	float GetNear( ) const { return m_zNear; }
	float GetFar( ) const { return m_zFar; }
	float GetAspect( ) const { return m_aspect; }
	float GetFov( ) const { return m_fov; }

	CRenderView( );
private:
	bool CreateDeviceDependentResource( IRenderer& renderer );

	std::vector<Viewport> m_viewportList;
	std::vector<RECT> m_scissorRectList;

	CXMFLOAT4X4 m_viewMatrix;
	CXMFLOAT4X4 m_projectionMatrix;

	RE_HANDLE m_gbufferConstantBuffer = RE_HANDLE_TYPE::INVALID_HANDLE;

	float m_zNear = 0.f;
	float m_zFar = 0.f;
	float m_aspect = 0.f;
	float m_fov = 0.f;
};

