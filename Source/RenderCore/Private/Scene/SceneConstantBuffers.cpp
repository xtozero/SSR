#include "stdafx.h"
#include "Scene/SceneConstantBuffers.h"

#include "ConstantSlotDefine.h"
#include "MultiThread/EngineTaskScheduler.h"
#include "Proxies/LightProxy.h"
#include "Proxies/PrimitiveProxy.h"
#include "RenderView.h"
#include "Scene/Scene.h"

#include <assert.h>

void SceneViewConstantBuffer::Initialize( )
{
	assert( IsInRenderThread() );

	m_constantBuffer = TypedConstatBuffer<ViewConstantBufferParameters>( );
}

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

	auto viewMatrix = XMMatrixLookToLH( view.m_viewOrigin, 
										view.m_viewAxis[2],
										view.m_viewAxis[1] );
	param.m_viewMatrix = XMMatrixTranspose( viewMatrix );

	auto projMatrix = XMMatrixPerspectiveFovLH( view.m_fov, 
												view.m_aspect, 
												view.m_nearPlaneDistance, 
												view.m_farPlaneDistance );
	param.m_projMatrix = XMMatrixTranspose( projMatrix );

	auto viewProjMatrix = XMMatrixMultiply( viewMatrix, projMatrix );
	param.m_viewProjMatrix = XMMatrixTranspose( viewProjMatrix );

	auto invViewMatrix = XMMatrixInverse( nullptr, viewMatrix );
	param.m_invViewMatrix = XMMatrixTranspose( invViewMatrix );

	auto invProjMatrix = XMMatrixInverse( nullptr, projMatrix );
	param.m_invProjMatrix = XMMatrixTranspose( invProjMatrix );

	auto invViewProjMatrix = XMMatrixInverse( nullptr, viewProjMatrix );
	param.m_invViewProjMatrix = XMMatrixTranspose( invViewProjMatrix );

	if ( scene && scene->HemisphereLight( ) )
	{
		const HemisphereLightProxy& hemisphereLight = *scene->HemisphereLight( );

		param.m_hemisphereLightLowerColor = hemisphereLight.LowerColor( );
		param.m_hemisphereLightUpperColor = hemisphereLight.UpperColor( );
		param.m_hemisphereLightUpVector = hemisphereLight.UpVector( );
	}
	else
	{
		param.m_hemisphereLightLowerColor = CXMFLOAT4();
		param.m_hemisphereLightUpperColor = CXMFLOAT4();
		param.m_hemisphereLightUpVector = CXMFLOAT3();
	}
}

PrimitiveSceneData::PrimitiveSceneData( const PrimitiveProxy* proxy )
{
	using namespace DirectX;

	if ( proxy )
	{
		m_worldMatrix = proxy->GetTransform( );
		m_invWorldMatrix = XMMatrixInverse( nullptr, m_worldMatrix );
	}
}

void ScenePrimitiveBuffer::Resize( uint32 size )
{
	m_buffer.Resize( size * sizeof( PrimitiveSceneData ) / sizeof( CXMFLOAT4 ), true );
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
