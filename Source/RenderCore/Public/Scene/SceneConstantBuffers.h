#pragma once

#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "ShaderArguments.h"
#include "SizedTypes.h"
#include "TypedBuffer.h"

namespace rendercore
{
	class PrimitiveProxy;
	class RenderViewGroup;
	class Scene;

	BEGIN_SHADER_ARGUMENTS_STRUCT( SceneViewParameters )
		DECLARE_VALUE( Matrix, ViewMatrix )
		DECLARE_VALUE( Matrix, ProjMatrix )
		DECLARE_VALUE( Matrix, ViewProjMatrix )

		DECLARE_VALUE( Matrix, InvViewMatrix )
		DECLARE_VALUE( Matrix, InvProjMatrix )
		DECLARE_VALUE( Matrix, InvViewProjMatrix )

		DECLARE_VALUE( Matrix, PrevViewMatrix )
		DECLARE_VALUE( Matrix, PrevProjMatrix )
		DECLARE_VALUE( Matrix, PrevViewProjMatrix )

		DECLARE_VALUE( float, NearPlaneDist )
		DECLARE_VALUE( float, FarPlaneDist )
		DECLARE_VALUE( float, ElapsedTime )
		DECLARE_VALUE( float, TotalTime )

		DECLARE_VALUE( Vector, CameraPos )
		DECLARE_VALUE( uint32, FrameCount )
		DECLARE_VALUE( Vector2, ViewportDimensions )
	END_SHADER_ARGUMENTS_STRUCT()

	struct PreviousFrameContext final
	{
		Matrix m_viewMatrix;
		Matrix m_projMatrix;
		Matrix m_viewProjMatrix;
	};

	SceneViewParameters GetViewParameters( const PreviousFrameContext* prevFrameContext, const RenderViewGroup& renderViewGroup, size_t viewIndex );

	class PrimitiveSceneData final
	{
	public:
		PrimitiveSceneData( const Scene& scene, uint32 primitiveId );

		Matrix m_worldMatrix;
		Matrix m_prevWorldMatrix;
		Matrix m_invWorldMatrix;
	};

	class ScenePrimitiveBuffer final
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
