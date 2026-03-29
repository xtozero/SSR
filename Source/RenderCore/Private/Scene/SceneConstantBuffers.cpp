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
	SceneViewParameters GetViewParameters( const PreviousFrameContext* prevFrameContext, const RenderViewGroup& renderViewGroup, size_t viewIndex )
	{
		SceneViewParameters params = {};

		const RenderView& view = renderViewGroup.GetRenderView( viewIndex );

		auto viewMatrix = LookFromMatrix( view.m_viewOrigin,
			view.m_viewAxis[2],
			view.m_viewAxis[1] );
		params.ViewMatrix = viewMatrix.GetTrasposed();

		auto projMatrix = PerspectiveMatrix( view.m_fov,
			view.m_aspect,
			view.m_nearPlaneDistance,
			view.m_farPlaneDistance );
		params.ProjMatrix = projMatrix.GetTrasposed();

		auto viewProjMatrix = viewMatrix * projMatrix;
		params.ViewProjMatrix = viewProjMatrix.GetTrasposed();

		auto invViewMatrix = viewMatrix.Inverse();
		params.InvViewMatrix = invViewMatrix.GetTrasposed();

		auto invProjMatrix = projMatrix.Inverse();
		params.InvProjMatrix = invProjMatrix.GetTrasposed();

		auto invViewProjMatrix = viewProjMatrix.Inverse();
		params.InvViewProjMatrix = invViewProjMatrix.GetTrasposed();

		if ( prevFrameContext )
		{
			params.PrevViewMatrix = prevFrameContext->m_viewMatrix.GetTrasposed();
			params.PrevProjMatrix = prevFrameContext->m_projMatrix.GetTrasposed();
			params.PrevViewProjMatrix = prevFrameContext->m_viewProjMatrix.GetTrasposed();
		}
		else
		{
			params.PrevViewMatrix = params.ViewMatrix;
			params.PrevProjMatrix = params.ProjMatrix;
			params.PrevViewProjMatrix = params.ViewProjMatrix;
		}

		params.NearPlaneDist = view.m_nearPlaneDistance;
		params.FarPlaneDist = view.m_farPlaneDistance;

		params.ElapsedTime = renderViewGroup.GetElapsedTime();
		params.TotalTime = renderViewGroup.GetTotalTime();
		params.CameraPos = view.m_viewOrigin;
		params.FrameCount = static_cast<uint32>( renderViewGroup.Scene().GetNumFrame() );

		auto wh = renderViewGroup.GetViewport().Size();
		params.ViewportDimensions = Vector2( static_cast<float>( wh.first ), static_cast<float>( wh.second ) );

		return params;
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
