#pragma once

#include "TypedBuffer.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "SizedTypes.h"

class PrimitiveProxy;
class RenderViewGroup;
class Scene;

struct ViewConstantBufferParameters
{
	Matrix m_viewMatrix;
	Matrix m_projMatrix;
	Matrix m_viewProjMatrix;

	Matrix m_invViewMatrix;
	Matrix m_invProjMatrix;
	Matrix m_invViewProjMatrix;

	ColorF m_hemisphereLightUpperColor;
	ColorF m_hemisphereLightLowerColor;
	Vector m_hemisphereLightUpVector;
	float padding1;
	float m_nearPlaneDist;
	float m_farPlaneDist;
	float m_elapsedTime;
	float m_totalTime;
};

void FillViewConstantParam( ViewConstantBufferParameters& param, const Scene* scene, const RenderViewGroup& renderViewGroup, size_t viewIndex );

class SceneViewConstantBuffer
{
public:
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

	Matrix m_worldMatrix;
	Matrix m_invWorldMatrix;
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
	TypedBuffer<Vector4> m_buffer;
};