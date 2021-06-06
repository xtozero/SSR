#include "stdafx.h"
#include "Scene/Scene.h"

#include "ByteBuffer.h"
#include "Components/PrimitiveComponent.h"
#include "MultiThread/EngineTaskScheduler.h"
#include "Proxies/PrimitiveProxy.h"
#include "Scene/PrimitiveSceneInfo.h"

#include <algorithm>

Scene::Scene( )
{
	if ( IsInRenderThread( ) )
	{
		m_viewConstant.Initialize( );
	}
	else
	{
		EnqueueRenderTask( [this]
		{
			m_viewConstant.Initialize( );
		} );
	}
}

void Scene::AddPrimitive( PrimitiveComponent* primitive )
{
	PrimitiveProxy* proxy = primitive->CreateProxy( );
	primitive->m_sceneProxy = proxy;

	if ( proxy == nullptr )
	{
		return;
	}

	PrimitiveSceneInfo* primitiveSceneInfo = new PrimitiveSceneInfo( proxy );
	primitiveSceneInfo->m_sceneProxy = proxy;

	proxy->m_primitiveSceneInfo = primitiveSceneInfo;

	struct AddPrimitiveSceneInfoParam
	{
		CXMFLOAT4X4 m_worldTransform;
	};
	AddPrimitiveSceneInfoParam param = {
		primitive->GetRenderMatrix( )
	};

	EnqueueRenderTask( [this, param, primitiveSceneInfo]( )
	{
		PrimitiveProxy* sceneProxy = primitiveSceneInfo->m_sceneProxy;

		sceneProxy->SetTransform( param.m_worldTransform );
		sceneProxy->CreateRenderData( );

		AddPrimitiveSceneInfo( primitiveSceneInfo );
	} );
}

void Scene::RemovePrimitive( PrimitiveComponent* primitive )
{
	PrimitiveProxy* proxy = primitive->m_sceneProxy;

	if ( proxy )
	{
		PrimitiveSceneInfo* primitiveSceneInfo = proxy->m_primitiveSceneInfo;
		primitive->m_sceneProxy = nullptr;

		EnqueueRenderTask( [this, primitiveSceneInfo]( )
		{
			RemovePrimitiveSceneInfo( primitiveSceneInfo );
		} );
	}
}

void Scene::DrawScene( const RenderViewGroup& views )
{
}

SHADING_METHOD Scene::ShadingMethod( ) const
{
	return SHADING_METHOD::Forward;
}

void Scene::AddPrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo )
{
	assert( IsInRenderThread() );
	assert( primitiveSceneInfo );

	m_primitiveToUpdate.push_back( m_primitives.size( ) );
	primitiveSceneInfo->m_id = m_primitives.size( );

	m_primitives.push_back( primitiveSceneInfo );
}

void Scene::RemovePrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo )
{
	assert( IsInRenderThread( ) );

	m_primitives.erase( std::remove( m_primitives.begin(), m_primitives.end(), primitiveSceneInfo ), m_primitives.end( ) );
	m_primitiveToUpdate.erase( std::remove( m_primitiveToUpdate.begin( ), m_primitiveToUpdate.end( ), primitiveSceneInfo->m_id ), m_primitiveToUpdate.end( ) );

	for ( std::size_t i = primitiveSceneInfo->m_id; i < m_primitives.size( ); ++i )
	{
		--m_primitives[i]->m_id;
	}

	delete primitiveSceneInfo->m_sceneProxy;
	delete primitiveSceneInfo;
}

bool UpdateGPUPrimitiveInfos( Scene& scene )
{
	assert( IsInRenderThread( ) );
	std::size_t updateSize = scene.m_primitiveToUpdate.size( );
	if ( updateSize == 0 )
	{
		return true;
	}

	DistributionCopyCS computeShader;
	ComputeShader* cs = computeShader.Shader( );
	if ( cs == nullptr )
	{
		return false;
	}

	std::size_t totalPrimitives = scene.m_primitives.size( );
	scene.m_gpuPrimitiveInfos.Resize( totalPrimitives );

	GpuMemcpy gpuMemcpy( updateSize, sizeof( PrimitiveSceneData ) / sizeof( CXMFLOAT4 ), scene.m_uploadPrimitiveBuffer, scene.m_distributionBuffer );

	for ( auto index : scene.m_primitiveToUpdate )
	{
		PrimitiveProxy* proxy = scene.m_primitives[index]->m_sceneProxy;

		PrimitiveSceneData param( proxy );
		gpuMemcpy.Add( (const char*)( &param ), index );
	}

	aga::Buffer* gpuPrimitiveInfos = scene.m_gpuPrimitiveInfos.Resource( );
	gpuMemcpy.Upload( gpuPrimitiveInfos );

	scene.m_primitiveToUpdate.clear( );
	return true;
}
