#include "stdafx.h"
#include "Scene/Scene.h"

#include "ByteBuffer.h"
#include "Components/LightComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/TexturedSkyComponent.h"
#include "Proxies/LightProxy.h"
#include "Proxies/PrimitiveProxy.h"
#include "Proxies/TexturedSkyProxy.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "TaskScheduler.h"

#include <algorithm>

Scene::Scene( )
{
	EnqueueRenderTask( [this]
	{
		m_viewConstant.Initialize( );
	} );
}

void Scene::AddPrimitive( PrimitiveComponent* primitive )
{
	PrimitiveProxy* proxy = primitive->CreateProxy( );
	primitive->m_sceneProxy = proxy;

	if ( proxy == nullptr )
	{
		return;
	}

	PrimitiveSceneInfo* primitiveSceneInfo = new PrimitiveSceneInfo( primitive, *this );

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

void Scene::AddTexturedSkyComponent( TexturedSkyComponent* texturedSky )
{
	TexturedSkyProxy* proxy = texturedSky->CreateProxy( );
	texturedSky->m_texturedSkyProxy = proxy;

	if ( proxy )
	{
		EnqueueRenderTask( [this, proxy]
		{
			proxy->CreateRenderData( );

			AddTexturedSky( proxy );
		} );
	}
}

void Scene::RemoveTexturedSkyComponent( TexturedSkyComponent* texturedSky )
{
	TexturedSkyProxy* proxy = texturedSky->m_texturedSkyProxy;

	if ( proxy )
	{
		texturedSky->m_texturedSkyProxy = nullptr;

		EnqueueRenderTask( [this, proxy]( )
		{
			RemoveTexturedSky( proxy );
		} );
	}
}

void Scene::AddHemisphereLightComponent( HemisphereLightComponent* light )
{
	HemisphereLightProxy* proxy = light->CreateProxy( );
	light->Proxy( ) = proxy;

	if ( proxy )
	{
		EnqueueRenderTask( [this, proxy]
		{
			AddHemisphereLight( proxy );
		} );
	}
}

void Scene::RemoveHemisphereLightComponent( HemisphereLightComponent* light )
{
	HemisphereLightProxy* proxy = light->Proxy( );

	if ( proxy )
	{
		light->Proxy( ) = nullptr;

		EnqueueRenderTask( [this, proxy]( )
		{
			RemoveHemisphereLight( proxy );
		} );
	}
}

void Scene::AddLight( LightComponent* light )
{
	LightProxy* proxy = light->CreateProxy( );
	light->m_lightProxy = proxy;

	if ( proxy == nullptr )
	{
		return;
	}

	LightSceneInfo* lightsceneInfo = new LightSceneInfo( *light, *this );

	proxy->m_lightSceneInfo = lightsceneInfo;

	EnqueueRenderTask( [this, lightsceneInfo]
	{
		AddLightSceneInfo( lightsceneInfo );
	} );
}

void Scene::RemoveLight( LightComponent* light )
{
	LightProxy* proxy = light->m_lightProxy;

	if ( proxy )
	{
		LightSceneInfo* lightSceneInfo = proxy->m_lightSceneInfo;
		light->m_lightProxy = nullptr;

		EnqueueRenderTask( [this, lightSceneInfo]( )
		{
			RemoveLightSceneInfo( lightSceneInfo );
		} );
	}
}

SHADING_METHOD Scene::ShadingMethod( ) const
{
	return SHADING_METHOD::Forward;
}

CachedDrawSnapshotInfo Scene::AddCachedDrawSnapshot( const DrawSnapshot& snapshot )
{
	CachedDrawSnapshotInfo info;
	info.m_snapshotBucketId = m_cachedSnapshotBuckect.Add( snapshot );
	info.m_snapshotIndex = m_cachedSnapshots.Add( snapshot );

	return info;
}

void Scene::RemoveCachedDrawSnapshot( const CachedDrawSnapshotInfo& info )
{
	m_cachedSnapshotBuckect.Remove( info.m_snapshotBucketId );
	m_cachedSnapshots.RemoveAt( info.m_snapshotIndex );
}

void Scene::AddPrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo )
{
	assert( IsInRenderThread() );
	assert( primitiveSceneInfo );

	uint32 primitiveId = static_cast<uint32>( m_primitives.Add( primitiveSceneInfo ) );
	primitiveSceneInfo->m_primitiveId = primitiveId;

	m_primitiveToUpdate.push_back( primitiveId );
	
	primitiveSceneInfo->AddToScene( );
}

void Scene::RemovePrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo )
{
	assert( IsInRenderThread( ) );

	m_primitives.RemoveAt( primitiveSceneInfo->m_primitiveId );
	m_primitiveToUpdate.erase( std::remove( m_primitiveToUpdate.begin( ), m_primitiveToUpdate.end( ), primitiveSceneInfo->m_primitiveId ), m_primitiveToUpdate.end( ) );

	primitiveSceneInfo->RemoveFromScene( );
	delete primitiveSceneInfo->m_sceneProxy;
	delete primitiveSceneInfo;
}

void Scene::AddTexturedSky( TexturedSkyProxy* texturedSky )
{
	m_texturedSky = texturedSky;
}

void Scene::RemoveTexturedSky( TexturedSkyProxy* texturedSky )
{
	delete m_texturedSky;
	m_texturedSky = nullptr;
}

void Scene::AddHemisphereLight( HemisphereLightProxy* hemisphereLight )
{
	m_hemisphereLight = hemisphereLight;
}

void Scene::RemoveHemisphereLight( HemisphereLightProxy* hemisphereLight )
{
	delete hemisphereLight;
	m_hemisphereLight = nullptr;
}

void Scene::AddLightSceneInfo( LightSceneInfo* lightSceneInfo )
{
	uint32 id = static_cast<uint32>( m_lights.Add( lightSceneInfo ) );
	lightSceneInfo->SetID( id );
}

void Scene::RemoveLightSceneInfo( LightSceneInfo* lightSceneInfo )
{
	uint32 id = lightSceneInfo->ID( );
	m_lights.RemoveAt( id );

	delete lightSceneInfo->Proxy( );
	delete lightSceneInfo;
}

bool UpdateGPUPrimitiveInfos( Scene& scene )
{
	assert( IsInRenderThread( ) );
	uint32 updateSize = static_cast<uint32>( scene.m_primitiveToUpdate.size( ) );
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

	uint32 totalPrimitives = static_cast<uint32>( scene.m_primitives.Size( ) );
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
