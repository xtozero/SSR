#pragma once

#include "TypedBuffer.h"
#include "Math/CXMFloat.h"
#include "SizedTypes.h"

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

	aga::Buffer* Resource( );
	const aga::Buffer* Resource( ) const;

private:
	TypedConstatBuffer<ViewConstantBufferParameters> m_constantBuffer;
};

class PrimitiveSceneData
{
public:
	explicit PrimitiveSceneData( const PrimitiveProxy* proxy );

	CXMFLOAT4X4 m_worldMatrix;
	CXMFLOAT4X4 m_invWorldMatrix;
};

class ScenePrimitiveBuffer
{
public:
	void Resize( uint32 size );

	aga::ShaderResourceView* SRV( );
	const aga::ShaderResourceView* SRV( ) const;

	aga::Buffer* Resource( );
	const aga::Buffer* Resource( ) const;

private:
	TypedBuffer<CXMFLOAT4> m_buffer;
};