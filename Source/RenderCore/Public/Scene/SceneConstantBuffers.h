#pragma once

#include "TypedBuffer.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "SizedTypes.h"

namespace rendercore
{
	class PrimitiveProxy;
	class RenderViewGroup;
	class Scene;

	struct SceneViewParameters
	{
		Matrix m_viewMatrix;
		Matrix m_projMatrix;
		Matrix m_viewProjMatrix;

		Matrix m_invViewMatrix;
		Matrix m_invProjMatrix;
		Matrix m_invViewProjMatrix;

		Matrix m_prevViewMatrix;
		Matrix m_prevProjMatrix;
		Matrix m_prevViewProjMatrix;

		ColorF m_hemisphereLightUpperColor;
		ColorF m_hemisphereLightLowerColor;
		Vector4 m_hemisphereLightUpVector;
		float m_nearPlaneDist;
		float m_farPlaneDist;
		float m_elapsedTime;
		float m_totalTime;
		uint32 m_frameCount;
		Vector2 m_viewportDimensions;
		uint32 padding;
	};

	struct PreviousFrameContext
	{
		Matrix m_viewMatrix;
		Matrix m_projMatrix;
		Matrix m_viewProjMatrix;
	};

	SceneViewParameters FillViewConstantParam( const Scene* scene, const PreviousFrameContext* prevFrameContext, const RenderViewGroup& renderViewGroup, size_t viewIndex );

	class SceneViewConstantBuffer
	{
	public:
		void Update( const SceneViewParameters& param );

		agl::Buffer* Resource();
		const agl::Buffer* Resource() const;

	private:
		TypedConstatBuffer<SceneViewParameters> m_constantBuffer;
	};

	class PrimitiveSceneData
	{
	public:
		PrimitiveSceneData( const Scene& scene, uint32 primitiveId );

		Matrix m_worldMatrix;
		Matrix m_prevWorldMatrix;
		Matrix m_invWorldMatrix;
	};

	class ScenePrimitiveBuffer
	{
	public:
		void Resize( uint32 size );

		agl::ShaderResourceView* SRV();
		const agl::ShaderResourceView* SRV() const;

		agl::Buffer* Resource();
		const agl::Buffer* Resource() const;

	private:
		TypedBuffer<Vector4> m_buffer;
	};
}
