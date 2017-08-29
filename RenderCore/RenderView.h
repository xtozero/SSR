#pragma once

#include "D3D11ConstantBuffer.h"
#include "IRenderView.h"
#include "../shared/Math/CXMFloat.h"

#include <d3d11.h>
#include <vector>

class RenderView : public IRenderView
{
public:
	bool initialize( ID3D11Device* pDevice );

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f ) override;
	virtual void PopViewPort( ) override;
	virtual void PushScissorRect( const RECT& rect ) override;
	virtual void PopScissorRect( ) override;

	void SetViewPort( ID3D11DeviceContext* pDeviceContext );
	void SetScissorRects( ID3D11DeviceContext* pDeviceContext );

	virtual void CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar ) override;
	virtual void CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar ) override;

	void UpdataView( ID3D11DeviceContext* pDeviceContext );

	virtual void SetViewMatrix( const CXMFLOAT4X4& viewMat ) override { m_viewMatrix = viewMat; }

	virtual const CXMFLOAT4X4& GetProjectionMatrix( ) override { return m_projectionMatrix; }
private:
	std::vector<D3D11_VIEWPORT> m_viewportList;
	std::vector<RECT> m_scissorRectList;

	CXMFLOAT4X4 m_viewMatrix;
	CXMFLOAT4X4 m_projectionMatrix;

	D3D11ConstantBuffer m_viewConstantBuffer;
	D3D11ConstantBuffer m_gBufferConstantBuffer;

	float m_zFar;
public:
	RenderView( );
	~RenderView( );
};

