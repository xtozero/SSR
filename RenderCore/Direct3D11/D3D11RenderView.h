#pragma once

#include "../CommonRenderer/IBuffer.h"
#include "../CommonRenderer/IRenderView.h"
#include "../../shared/Math/CXMFloat.h"

#include <d3d11.h>
#include <vector>

class IRenderer;

class D3D11RenderView : public IRenderView
{
public:
	bool initialize( IRenderer& renderer );

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f ) override;
	virtual void PopViewPort( ) override;
	virtual void PushScissorRect( const RECT& rect ) override;
	virtual void PopScissorRect( ) override;

	void SetViewPort( );
	void SetScissorRects( );

	virtual void CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar ) override;
	virtual void CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar ) override;

	void UpdataView( );

	virtual void SetViewMatrix( const CXMFLOAT4X4& viewMat ) override { m_viewMatrix = viewMat; }

	virtual const CXMFLOAT4X4& GetProjectionMatrix( ) override { return m_projectionMatrix; }
private:
	std::vector<D3D11_VIEWPORT> m_viewportList;
	std::vector<RECT> m_scissorRectList;

	CXMFLOAT4X4 m_viewMatrix;
	CXMFLOAT4X4 m_projectionMatrix;

	IBuffer* m_viewConstantBuffer = nullptr;
	IBuffer* m_gBufferConstantBuffer = nullptr;

	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	float m_zFar;
public:
	D3D11RenderView( ID3D11DeviceContext* pDeviceContext );
	~D3D11RenderView( );
};

