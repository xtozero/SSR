#include "stdafx.h"
#include "Scene/SceneConstantBuffers.h"

#include "ConstantSlotDefine.h"
#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"
#include "Proxies/PrimitiveProxy.h"
#include "RenderView.h"

#include <assert.h>

void SceneViewConstantBuffer::Initialize( )
{
	assert( IsInRenderThread() );

	m_constantBuffer = TypedConstatBuffer<ViewConstantBufferParameters>::Create( );
}

void SceneViewConstantBuffer::Update( const ViewConstantBufferParameters& param )
{
	assert( IsInRenderThread( ) );

	m_constantBuffer.Update( param );
}

void SceneViewConstantBuffer::Bind( )
{
	m_constantBuffer.Bind( SHADER_TYPE::VS, VS_CONSTANT_BUFFER::VIEW_PROJECTION );
}

void FillViewConstantParam( ViewConstantBufferParameters& param, const RenderView& view )
{
	using namespace DirectX;

	auto viewMatrix = XMMatrixLookToLH( 
		view.m_viewOrigin, 
		view.m_viewAxis[2],
		view.m_viewAxis[1] );
	param.m_viewMatrix = viewMatrix;

	auto projMatrix = XMMatrixPerspectiveFovLH( view.m_fov, 
		view.m_aspect, 
		view.m_nearPlaneDistance, 
		view.m_farPlaneDistance );
	param.m_projMatrix = projMatrix;

	param.m_viewProjMatrix = XMMatrixMultiply( viewMatrix, projMatrix );

	param.m_invViewMatrix = XMMatrixInverse( nullptr, viewMatrix );
	param.m_invProjMatrix = XMMatrixInverse( nullptr, projMatrix );
	param.m_invViewProjMatrix = XMMatrixInverse( nullptr, param.m_viewProjMatrix );
}

PrimitiveBufferParameters::PrimitiveBufferParameters( const PrimitiveProxy* proxy )
{
	if ( proxy )
	{
		m_worldMatrix = proxy->GetTransform( );
	}
}

void ScenePrimitiveBuffer::Resize( std::size_t size )
{
	m_buffer.Resize( size * sizeof( PrimitiveBufferParameters ) / sizeof( CXMFLOAT4 ) );
}
