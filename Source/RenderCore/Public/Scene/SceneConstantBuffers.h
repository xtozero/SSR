#pragma once

#include "TypedBuffer.h"
#include "Math/CXMFloat.h"
#include "SizedTypes.h"

class PrimitiveProxy;
class Scene;
struct RenderView;

struct alignas(16) ViewConstantBufferParameters
{
	CXMFLOAT4X4 m_viewMatrix;
	CXMFLOAT4X4 m_projMatrix;
	CXMFLOAT4X4 m_viewProjMatrix;

	CXMFLOAT4X4 m_invViewMatrix;
	CXMFLOAT4X4 m_invProjMatrix;
	CXMFLOAT4X4 m_invViewProjMatrix;

	CXMFLOAT4 m_hemisphereLightUpperColor;
	CXMFLOAT4 m_hemisphereLightLowerColor;
	CXMFLOAT3 m_hemisphereLightUpVector;
	float padding3;
};

void FillViewConstantParam( ViewConstantBufferParameters& param, const Scene* scene, const RenderView& view );

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