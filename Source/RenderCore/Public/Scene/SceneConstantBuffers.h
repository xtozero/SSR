#pragma once

#include "GraphicsResource/Buffer.h"
#include "Math/CXMFloat.h"

struct ViewConstantBufferParameters
{
	CXMFLOAT4X4 m_viewMatrix;
	CXMFLOAT4X4 m_projMatrix;
	CXMFLOAT4X4 m_viewProjMatrix;
};

class SceneConstantBuffers
{
public:
	void Initialize( );

private:
	TypeConstatBuffer<ViewConstantBufferParameters> m_viewConstantBuffer;
};