#pragma once

#include "TypedBuffer.h"
#include "Math/CXMFloat.h"

class PrimitiveProxy;
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

class SceneViewConstantBuffer
{
public:
	void Initialize( );
	void Update( const ViewConstantBufferParameters& param );
	void Bind( );

private:
	TypedConstatBuffer<ViewConstantBufferParameters> m_constantBuffer;
};

class PrimitiveBufferParameters
{
public:
	explicit PrimitiveBufferParameters( const PrimitiveProxy* proxy );

	CXMFLOAT4X4 m_worldMatrix;
};

class ScenePrimitiveBuffer
{
public:
	void Resize( std::size_t size );

	operator aga::Buffer*( )
	{
		return m_buffer;
	}

private:
	std::size_t m_size = 0;
	TypedBuffer<CXMFLOAT4> m_buffer;
};