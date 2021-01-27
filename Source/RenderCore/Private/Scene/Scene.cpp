#include "stdafx.h"
#include "Scene/Scene.h"

#include "Components/PrimitiveComponent.h"
#include "MultiThread/EngineTaskScheduler.h"
#include "Proxies/PrimitiveProxy.h"
#include "Scene/PrimitiveSceneInfo.h"

#include <algorithm>

Scene::Scene( )
{
	EnqueueRenderTask( [this]
	{
		m_constantBuffers.Initialize( );
	});
}

void Scene::AddPrimitive( PrimitiveComponent* primitive )
{
	PrimitiveProxy* proxy = primitive->CreateProxy( );
	primitive->m_sceneProxy = proxy;

	if ( proxy == nullptr )
	{
		return;
	}

	PrimitivieSceneInfo* primitiveSceneInfo = new PrimitivieSceneInfo( proxy );
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

		AddPrimitiveSceneInfo( primitiveSceneInfo );
	} );
}

void Scene::RemovePrimitive( PrimitiveComponent* primitive )
{
	PrimitiveProxy* proxy = primitive->m_sceneProxy;

	if ( proxy )
	{
		PrimitivieSceneInfo* primitiveSceneInfo = proxy->m_primitiveSceneInfo;
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

void Scene::AddPrimitiveSceneInfo( PrimitivieSceneInfo* primitiveSceneInfo )
{
	assert( IsInRenderThread() );

	m_primitives.push_back( primitiveSceneInfo );
}

void Scene::RemovePrimitiveSceneInfo( PrimitivieSceneInfo* primitiveSceneInfo )
{
	assert( IsInRenderThread( ) );

	m_primitives.erase( std::remove( m_primitives.begin(), m_primitives.end(), primitiveSceneInfo ) );
	delete primitiveSceneInfo->m_sceneProxy;
	delete primitiveSceneInfo;
}
