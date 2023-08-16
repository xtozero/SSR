#include "stdafx.h"
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
	void SceneViewConstantBuffer::Update( const SceneViewParameters& param )
	{
		assert( IsInRenderThread() );

		m_constantBuffer.Update( param );
	}

	agl::Buffer* SceneViewConstantBuffer::Resource()
	{
		return m_constantBuffer.Resource();
	}

	const agl::Buffer* SceneViewConstantBuffer::Resource() const
	{
		return m_constantBuffer.Resource();
	}

	SceneViewParameters FillViewConstantParam( const Scene* scene, const PreviousFrameContext* prevFrameContext, const RenderViewGroup& renderViewGroup, size_t viewIndex )
	{
		SceneViewParameters param = {};

		const RenderView& view = renderViewGroup[viewIndex];

		auto viewMatrix = LookFromMatrix( view.m_viewOrigin,
			view.m_viewAxis[2],
			view.m_viewAxis[1] );
		param.m_viewMatrix = viewMatrix.GetTrasposed();

		auto projMatrix = PerspectiveMatrix( view.m_fov,
			view.m_aspect,
			view.m_nearPlaneDistance,
			view.m_farPlaneDistance );
		param.m_projMatrix = projMatrix.GetTrasposed();

		auto viewProjMatrix = viewMatrix * projMatrix;
		param.m_viewProjMatrix = viewProjMatrix.GetTrasposed();

		auto invViewMatrix = viewMatrix.Inverse();
		param.m_invViewMatrix = invViewMatrix.GetTrasposed();

		auto invProjMatrix = projMatrix.Inverse();
		param.m_invProjMatrix = invProjMatrix.GetTrasposed();

		auto invViewProjMatrix = viewProjMatrix.Inverse();
		param.m_invViewProjMatrix = invViewProjMatrix.GetTrasposed();

		if ( prevFrameContext )
		{
			param.m_prevViewMatrix = prevFrameContext->m_viewMatrix.GetTrasposed();
			param.m_prevProjMatrix = prevFrameContext->m_projMatrix.GetTrasposed();
			param.m_prevViewProjMatrix = prevFrameContext->m_viewProjMatrix.GetTrasposed();
		}
		else
		{
			param.m_prevViewMatrix = param.m_viewMatrix;
			param.m_prevProjMatrix = param.m_projMatrix;
			param.m_prevViewProjMatrix = param.m_viewProjMatrix;
		}

		if ( scene && scene->HemisphereLight() )
		{
			const HemisphereLightProxy& hemisphereLight = *scene->HemisphereLight();

			param.m_hemisphereLightLowerColor = hemisphereLight.LowerColor();
			param.m_hemisphereLightUpperColor = hemisphereLight.UpperColor();
			param.m_hemisphereLightUpVector = hemisphereLight.UpVector();
		}
		else
		{
			param.m_hemisphereLightLowerColor = Vector::ZeroVector;
			param.m_hemisphereLightUpperColor = Vector::ZeroVector;
			param.m_hemisphereLightUpVector = Vector4::ZeroVector;
		}

		param.m_nearPlaneDist = view.m_nearPlaneDistance;
		param.m_farPlaneDist = view.m_farPlaneDistance;

		param.m_elapsedTime = renderViewGroup.GetElapsedTime();
		param.m_totalTime = renderViewGroup.GetTotalTime();
		param.m_cameraPos = view.m_viewOrigin;
		param.m_frameCount = scene ? static_cast<uint32>( scene->GetNumFrame() ) : 0;

		auto wh = renderViewGroup.GetViewport().Size();
		param.m_viewportDimensions = Vector2( static_cast<float>( wh.first ), static_cast<float>( wh.second ) );

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
