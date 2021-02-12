#pragma once

#include "TypedBuffer.h"
#include "Math/CXMFloat.h"

struct RenderView;

struct ViewConstantBufferParameters
{
	CXMFLOAT4X4 m_viewMatrix;
	CXMFLOAT4X4 m_projMatrix;
	CXMFLOAT4X4 m_viewProjMatrix;

	CXMFLOAT4X4 m_invViewMatrix;
	CXMFLOAT4X4 m_invProjMatrix;
	CXMFLOAT4X4 m_invViewProjMatrix;
};

void FillViewConstantParam( ViewConstantBufferParameters& param, const RenderView& view );

class SceneConstantBuffers
{
public:
	void Initialize( );
	void Update( const ViewConstantBufferParameters& param );
	void Bind( );

private:
	TypedConstatBuffer<ViewConstantBufferParameters> m_viewConstantBuffer;
};