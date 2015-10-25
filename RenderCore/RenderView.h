#pragma once

#include "D3D11ConstantBuffer.h"
#include <d3d11.h>
#include <D3DX9math.h>
#include "IRenderView.h"
#include <vector>

struct VIEW_PROJECTION
{
	D3DXMATRIX m_view;
	D3DXMATRIX m_projection;
};

class RenderView : public IRenderView
{
public:
	bool initialize( ID3D11Device* pDevice );

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f );
	virtual void PopViewPort( );
	void SetViewPort( ID3D11DeviceContext* pDeviceContext );

	virtual void CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar );
	virtual void CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar );

	void UpdataView( ID3D11DeviceContext* pDeviceContext );

	virtual void SetViewMatrix( const D3DXMATRIX& viewMat ) { m_viewMatrix = viewMat; }
private:
	std::vector<D3D11_VIEWPORT> m_viewportList;

	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projectionMatrix;

	D3D11ConstantBuffer m_viewConstantBuffer;
public:
	RenderView( );
	~RenderView( );
};

