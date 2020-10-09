#include "stdafx.h"
#include "Scene/Scene.h"

#include "Components/PrimitiveComponent.h"
#include "MultiThread/EngineTaskScheduler.h"
#include "Proxies/PrimitiveProxy.h"
#include "Scene/PrimitiveSceneInfo.h"

#include <algorithm>

void Scene::AddPrimitive( PrimitiveComponent* primitive )
{
	PrimitiveProxy* proxy = primitive->CreateProxy( );
	primitive->m_sceneProxy = proxy;

	if ( proxy == nullptr )
	{
		return;
	}

	PrimitivieSceneInfo* primitiveSceneInfo = new PrimitivieSceneInfo( proxy );
	primitiveSceneInfo->m_proxy = proxy;

	proxy->m_primitiveSceneInfo = primitiveSceneInfo;

	CXMFLOAT4X4 renderMatrix = primitive->GetRenderMatrix( );

	ENQUEUE_THREAD_TASK<ThreadType::RenderThread>( [this, primitiveSceneInfo]( )
	{
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

		ENQUEUE_THREAD_TASK<ThreadType::RenderThread>( [this, primitiveSceneInfo]( )
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
	delete primitiveSceneInfo->m_proxy;
	delete primitiveSceneInfo;
}
