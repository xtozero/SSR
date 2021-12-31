#include "stdafx.h"
#include "Scene/SceneConstantBuffers.h"

#include "Math/TransformationMatrix.h"
#include "Proxies/LightProxy.h"
#include "Proxies/PrimitiveProxy.h"
#include "RenderView.h"
#include "Scene/Scene.h"
#include "TaskScheduler.h"

#include <assert.h>

void SceneViewConstantBuffer::Update( const ViewConstantBufferParameters& param )
{
	assert( IsInRenderThread( ) );

	m_constantBuffer.Update( param );
}

aga::Buffer* SceneViewConstantBuffer::Resource( )
{
	return m_constantBuffer.Resource( );
}

const aga::Buffer* SceneViewConstantBuffer::Resource( ) const
{
	return m_constantBuffer.Resource( );
}

void FillViewConstantParam( ViewConstantBufferParameters& param, const Scene* scene, const RenderView& view )
{
	using namespace DirectX;

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

	if ( scene && scene->HemisphereLight( ) )
	{
		const HemisphereLightProxy& hemisphereLight = *scene->HemisphereLight( );

		param.m_hemisphereLightLowerColor = hemisphereLight.LowerColor( );
		param.m_hemisphereLightUpperColor = hemisphereLight.UpperColor( );
		param.m_hemisphereLightUpVector = hemisphereLight.UpVector( );
	}
	else
	{
		param.m_hemisphereLightLowerColor = Vector::ZeroVector;
		param.m_hemisphereLightUpperColor = Vector::ZeroVector;
		param.m_hemisphereLightUpVector = Vector::ZeroVector;
	}

	param.m_nearPlaneDist = view.m_nearPlaneDistance;
	param.m_farPlaneDist = view.m_farPlaneDistance;
}

PrimitiveSceneData::PrimitiveSceneData( const PrimitiveProxy* proxy )
{
	using namespace DirectX;

	if ( proxy )
	{
		m_worldMatrix = proxy->WorldTransform();
		m_invWorldMatrix = m_worldMatrix.Inverse();
	}
}

void ScenePrimitiveBuffer::Resize( uint32 size )
{
	m_buffer.Resize( size * sizeof( PrimitiveSceneData ) / sizeof( Vector4 ), true );
}

aga::ShaderResourceView* ScenePrimitiveBuffer::SRV( )
{
	return m_buffer.SRV( );
}

const aga::ShaderResourceView* ScenePrimitiveBuffer::SRV( ) const
{
	return m_buffer.SRV( );
}

aga::Buffer* ScenePrimitiveBuffer::Resource( )
{
	return m_buffer.Resource( );
}

const aga::Buffer* ScenePrimitiveBuffer::Resource( ) const
{
	return m_buffer.Resource( );
}
