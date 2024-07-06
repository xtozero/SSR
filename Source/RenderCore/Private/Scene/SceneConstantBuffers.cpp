#include "Scene/SceneConstantBuffers.h"

#include "Math/TransformationMatrix.h"
#include "Proxies/LightProxy.h"
#include "Proxies/PrimitiveProxy.h"
#include "RenderView.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "TaskScheduler.h"
#include "Viewport.h"

#include <cassert>

namespace rendercore
{
	SceneViewParameters GetViewParameters( const Scene* scene, const PreviousFrameContext* prevFrameContext, const RenderViewGroup& renderViewGroup, size_t viewIndex )
	{
		SceneViewParameters param = {};

		const RenderView& view = renderViewGroup[viewIndex];

		auto viewMatrix = LookFromMatrix( view.m_viewOrigin,
			view.m_viewAxis[2],
			view.m_viewAxis[1] );
		param.ViewMatrix = viewMatrix.GetTrasposed();

		auto projMatrix = PerspectiveMatrix( view.m_fov,
			view.m_aspect,
			view.m_nearPlaneDistance,
			view.m_farPlaneDistance );
		param.ProjMatrix = projMatrix.GetTrasposed();

		auto viewProjMatrix = viewMatrix * projMatrix;
		param.ViewProjMatrix = viewProjMatrix.GetTrasposed();

		auto invViewMatrix = viewMatrix.Inverse();
		param.InvViewMatrix = invViewMatrix.GetTrasposed();

		auto invProjMatrix = projMatrix.Inverse();
		param.InvProjMatrix = invProjMatrix.GetTrasposed();

		auto invViewProjMatrix = viewProjMatrix.Inverse();
		param.InvViewProjMatrix = invViewProjMatrix.GetTrasposed();

		if ( prevFrameContext )
		{
			param.PrevViewMatrix = prevFrameContext->m_viewMatrix.GetTrasposed();
			param.PrevProjMatrix = prevFrameContext->m_projMatrix.GetTrasposed();
			param.PrevViewProjMatrix = prevFrameContext->m_viewProjMatrix.GetTrasposed();
		}
		else
		{
			param.PrevViewMatrix = param.ViewMatrix;
			param.PrevProjMatrix = param.ProjMatrix;
			param.PrevViewProjMatrix = param.ViewProjMatrix;
		}

		param.NearPlaneDist = view.m_nearPlaneDistance;
		param.FarPlaneDist = view.m_farPlaneDistance;

		param.ElapsedTime = renderViewGroup.GetElapsedTime();
		param.TotalTime = renderViewGroup.GetTotalTime();
		param.CameraPos = view.m_viewOrigin;
		param.FrameCount = scene ? static_cast<uint32>( scene->GetNumFrame() ) : 0;

		auto wh = renderViewGroup.GetViewport().Size();
		param.ViewportDimensions = Vector2( static_cast<float>( wh.first ), static_cast<float>( wh.second ) );

		return param;
	}

	PrimitiveSceneData::PrimitiveSceneData( const Scene& scene, uint32 primitiveId )
	{
		const PrimitiveSceneInfo* primitiveSceneInfo = scene.Primitives()[primitiveId];
		if ( primitiveSceneInfo == nullptr )
		{
			return;
		}

		const PrimitiveProxy* proxy = primitiveSceneInfo->Proxy();
		if ( proxy )
		{
			m_worldMatrix = proxy->WorldTransform();

			std::optional<Matrix> prevWorldTransform = scene.GetPreviousTransform( primitiveId );
			if ( prevWorldTransform.has_value() )
			{
				m_prevWorldMatrix = prevWorldTransform.value();
			}
			else
			{
				m_prevWorldMatrix = m_worldMatrix;
			}

			m_invWorldMatrix = m_worldMatrix.Inverse();
		}
	}

	void ScenePrimitiveBuffer::Resize( uint32 size )
	{
		m_buffer.Resize( size * sizeof( PrimitiveSceneData ) / sizeof( Vector4 ), true );
	}

	agl::ShaderResourceView* ScenePrimitiveBuffer::SRV()
	{
		return m_buffer.SRV();
	}

	const agl::ShaderResourceView* ScenePrimitiveBuffer::SRV() const
	{
		return m_buffer.SRV();
	}

	agl::Buffer* ScenePrimitiveBuffer::Resource()
	{
		return m_buffer.Resource();
	}

	const agl::Buffer* ScenePrimitiveBuffer::Resource() const
	{
		return m_buffer.Resource();
	}
}
