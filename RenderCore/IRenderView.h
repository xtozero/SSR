#pragma once

class IRenderView
{
public:
	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f ) = 0;
	virtual void PopViewPort( ) = 0;

	virtual void CreatePerspectiveFovLHMatrix( float fov, float aspect, float zNear, float zFar ) = 0;
	virtual void CreatePerspectiveFovRHMatrix( float fov, float aspect, float zNear, float zFar ) = 0;

	virtual void SetViewMatrix( const D3DXMATRIX& viewMat ) = 0;

	virtual const D3DXMATRIX& GetProjectionMatrix( ) = 0;
protected:
	IRenderView( ) {}

public:
	virtual ~IRenderView( ) {}
};