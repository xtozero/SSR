#include "stdafx.h"
#include "Scene/Scene.h"

#include "ByteBuffer.h"
#include "Components/LightComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/TexturedSkyComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Physics/BoxSphereBounds.h"
#include "Proxies/LightProxy.h"
#include "Proxies/PrimitiveProxy.h"
#include "Proxies/SkyAtmosphereProxy.h"
#include "Proxies/TexturedSkyProxy.h"
#include "Proxies/VolumetricCloudProxy.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/VolumetricCloudSceneInfo.h"
#include "SkyAtmosphereRendering.h"
#include "TaskScheduler.h"

#include <algorithm>

namespace rendercore
{
	void Scene::AddPrimitive( PrimitiveComponent* primitive )
	{
		PrimitiveProxy* proxy = primitive->CreateProxy();
		primitive->m_sceneProxy = proxy;

		if ( proxy == nullptr )
		{
			return;
		}

		PrimitiveSceneInfo* primitiveSceneInfo = new PrimitiveSceneInfo( primitive, *this );

		proxy->m_primitiveSceneInfo = primitiveSceneInfo;

		struct AddPrimitiveSceneInfoParam
		{
			Matrix m_worldTransform;
			BoxSphereBounds m_worldBounds;
			BoxSphereBounds m_localBounds;
		};
		AddPrimitiveSceneInfoParam param = {
			primitive->GetRenderMatrix(),
			primitive->Bounds(),
			primitive->CalcBounds( Matrix::Identity ),
		};

		EnqueueRenderTask(
			[this, param, primitiveSceneInfo]()
			{
				PrimitiveProxy* sceneProxy = primitiveSceneInfo->Proxy();

				sceneProxy->WorldTransform() = param.m_worldTransform;
				sceneProxy->Bounds() = param.m_worldBounds;
				sceneProxy->LocalBounds() = param.m_localBounds;
				sceneProxy->CreateRenderData();

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

			EnqueueRenderTask(
				[this, primitiveSceneInfo]()
				{
					RemovePrimitiveSceneInfo( primitiveSceneInfo );
				} );
		}
	}

	void Scene::UpdatePrimitiveTransform( PrimitiveComponent* primitive )
	{
		PrimitiveProxy* proxy = primitive->m_sceneProxy;

		if ( proxy )
		{
			struct PrimitiveUpdateParam
			{
				Matrix m_worldTransform;
				BoxSphereBounds m_worldBounds;
				BoxSphereBounds m_localBounds;
			};
			PrimitiveUpdateParam param = {
				primitive->GetRenderMatrix(),
				primitive->Bounds(),
				primitive->CalcBounds( Matrix::Identity ),
			};

			EnqueueRenderTask(
				[this, param, proxy]()
				{
					m_velocityData.UpdateTransform( GetNumFrame(), proxy->PrimitiveId(), proxy->WorldTransform() );

					proxy->WorldTransform() = param.m_worldTransform;
					proxy->Bounds() = param.m_worldBounds;
					proxy->LocalBounds() = param.m_localBounds;

					AddPrimitiveToUpdate( proxy->PrimitiveId() );
				} );
		}
		else
		{
			AddPrimitive( primitive );
		}
	}

	void Scene::AddTexturedSkyComponent( TexturedSkyComponent* texturedSky )
	{
		TexturedSkyProxy* proxy = texturedSky->CreateProxy();
		texturedSky->m_texturedSkyProxy = proxy;

		if ( proxy )
		{
			EnqueueRenderTask(
				[this, proxy]
				{
					proxy->CreateRenderData();

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

			EnqueueRenderTask(
				[this, proxy]()
				{
					RemoveTexturedSky( proxy );
				} );
		}
	}

	void Scene::AddSkyAtmosphere( SkyAtmospherePorxy* skyAtmosphereProxy )
	{
		if ( skyAtmosphereProxy )
		{
			EnqueueRenderTask(
				[scene = this, skyAtmosphereProxy]()
				{
					skyAtmosphereProxy->RenderSceneInfo() = new SkyAtmosphereRenderSceneInfo();
					scene->m_skyAtmosphere = skyAtmosphereProxy->RenderSceneInfo();
					InitAtmosphereForScene( *scene );
				} );
		}
	}

	void Scene::RemoveAtomosphere( SkyAtmospherePorxy* skyAtmosphereProxy )
	{
		if ( skyAtmosphereProxy )
		{
			EnqueueRenderTask(
				[this, skyAtmosphereProxy]()
				{
					if ( m_skyAtmosphere == skyAtmosphereProxy->RenderSceneInfo() )
					{
						delete m_skyAtmosphere;
						m_skyAtmosphere = nullptr;
					}
				} );
		}
	}

	void Scene::AddVolumetricCloud( VolumetricCloudComponent* volumetricCloud )
	{
		VolumetricCloudProxy* proxy = volumetricCloud->CreateProxy();
		volumetricCloud->Proxy() = proxy;

		if ( proxy == nullptr )
		{
			return;
		}

		auto volumetricCloudSceneInfo = new VolumetricCloudSceneInfo( proxy );

		proxy->m_volumetricCloudSceneInfo = volumetricCloudSceneInfo;

		EnqueueRenderTask(
			[this, volumetricCloudSceneInfo]
			{
				volumetricCloudSceneInfo->CreateRenderData();

				AddVolumetricCloud( volumetricCloudSceneInfo );
			} );
	}

	void Scene::RemoveVolumetricCloud( VolumetricCloudComponent* volumetricCloud )
	{
		VolumetricCloudProxy* proxy = volumetricCloud->Proxy();

		if ( proxy )
		{
			VolumetricCloudSceneInfo* volumetricCloudSceneInfo = proxy->m_volumetricCloudSceneInfo;
			volumetricCloud->Proxy() = nullptr;

			EnqueueRenderTask(
				[this, volumetricCloudSceneInfo]
				{
					RemoveVolumetricCloud( volumetricCloudSceneInfo );
				} );
		}
	}

	void Scene::AddHemisphereLightComponent( HemisphereLightComponent* light )
	{
		HemisphereLightProxy* proxy = light->CreateProxy();
		light->Proxy() = proxy;

		if ( proxy )
		{
			EnqueueRenderTask(
				[this, proxy]
				{
					AddHemisphereLight( proxy );
				} );
		}
	}

	void Scene::RemoveHemisphereLightComponent( HemisphereLightComponent* light )
	{
		HemisphereLightProxy* proxy = light->Proxy();

		if ( proxy )
		{
			light->Proxy() = nullptr;

			EnqueueRenderTask(
				[this, proxy]()
				{
					RemoveHemisphereLight( proxy );
				} );
		}
	}

	void Scene::AddLight( LightComponent* light )
	{
		LightProxy* proxy = light->CreateProxy();
		light->m_lightProxy = proxy;

		if ( proxy == nullptr )
		{
			return;
		}

		LightSceneInfo* lightsceneInfo = new LightSceneInfo( *light, *this );

		proxy->m_lightSceneInfo = lightsceneInfo;

		EnqueueRenderTask(
			[this, lightsceneInfo]
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

			EnqueueRenderTask(
				[this, lightSceneInfo]()
				{
					RemoveLightSceneInfo( lightSceneInfo );
				} );
		}
	}

	ShadingMethod Scene::GetShadingMethod() const
	{
		return ShadingMethod::Forward;
	}

	void Scene::OnBeginSceneRendering()
	{
		++m_internalNumFrame;

		m_velocityData.StartFrame( *this );
	}

	void Scene::AddPrimitiveToUpdate( uint32 primitiveId )
	{
		m_primitiveToUpdate.emplace( primitiveId );
	}

	CachedDrawSnapshotInfo Scene::AddCachedDrawSnapshot( RenderPass passType, const DrawSnapshot& snapshot )
	{
		CachedDrawSnapshotInfo info;
		info.m_renderPass = passType;
		info.m_snapshotBucketId = m_cachedSnapshotBuckect.Add( snapshot );
		info.m_snapshotIndex = m_cachedSnapshots[static_cast<uint32>( passType )].Add( snapshot );

		return info;
	}

	void Scene::RemoveCachedDrawSnapshot( const CachedDrawSnapshotInfo& info )
	{
		uint32 passType = static_cast<uint32>( info.m_renderPass );
		m_cachedSnapshotBuckect.Remove( info.m_snapshotBucketId );
		m_cachedSnapshots[passType].RemoveAt( info.m_snapshotIndex );
	}

	std::optional<Matrix> Scene::GetPreviousTransform( uint32 primitiveId ) const
	{
		assert( IsInRenderThread() );
		return m_velocityData.GetPreviousTransform( primitiveId );
	}

	void Scene::AddPrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo )
	{
		assert( IsInRenderThread() );
		assert( primitiveSceneInfo );

		uint32 primitiveId = static_cast<uint32>( m_primitives.Add( primitiveSceneInfo ) );
		primitiveSceneInfo->PrimitiveId() = primitiveId;
		m_primitiveBounds.AddUninitialized();

		AddPrimitiveToUpdate( primitiveId );

		m_velocityData.UpdateTransform( GetNumFrame(), primitiveId, primitiveSceneInfo->Proxy()->WorldTransform() );

		primitiveSceneInfo->AddToScene();
	}

	void Scene::RemovePrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo )
	{
		assert( IsInRenderThread() );

		uint32 primitiveId = primitiveSceneInfo->PrimitiveId();

		m_primitives.RemoveAt( primitiveId );
		m_primitiveBounds.RemoveAt( primitiveId );
		m_primitiveToUpdate.erase( primitiveId );

		m_velocityData.RemoveFromScene( primitiveId );

		primitiveSceneInfo->RemoveFromScene();
		delete primitiveSceneInfo->Proxy();
		delete primitiveSceneInfo;
	}

	void Scene::AddTexturedSky( TexturedSkyProxy* texturedSky )
	{
		m_texturedSky = texturedSky;
	}

	void Scene::RemoveTexturedSky( [[maybe_unused]] TexturedSkyProxy* texturedSky )
	{
		delete m_texturedSky;
		m_texturedSky = nullptr;
	}

	void Scene::AddVolumetricCloud( VolumetricCloudSceneInfo* volumetricCloudSceneInfo )
	{
		assert( m_volumetricCloud == nullptr );

		m_volumetricCloud = volumetricCloudSceneInfo;
	}

	void Scene::RemoveVolumetricCloud( [[maybe_unused]] VolumetricCloudSceneInfo* volumetricCloudSceneInfo )
	{
		assert( m_volumetricCloud == volumetricCloudSceneInfo );

		m_volumetricCloud = nullptr;

		delete volumetricCloudSceneInfo->Proxy();
		delete volumetricCloudSceneInfo;
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

		lightSceneInfo->AddToScene();

		AddSkyAtmosphereLight( lightSceneInfo );
	}

	void Scene::RemoveLightSceneInfo( LightSceneInfo* lightSceneInfo )
	{
		RemoveSkyAtmosphereLight( lightSceneInfo );

		uint32 id = lightSceneInfo->ID();
		m_lights.RemoveAt( id );

		lightSceneInfo->RemoveFromScene();

		delete lightSceneInfo->Proxy();
		delete lightSceneInfo;
	}

	void Scene::AddSkyAtmosphereLight( LightSceneInfo* lightSceneInfo )
	{
		if ( lightSceneInfo->Proxy()->IsUsedAsAtmosphereSunLight() )
		{
			m_skyAtmosphereLight = lightSceneInfo;
		}
	}

	void Scene::RemoveSkyAtmosphereLight( LightSceneInfo* lightSceneInfo )
	{
		if ( m_skyAtmosphereLight == lightSceneInfo )
		{
			m_skyAtmosphereLight = nullptr;

			for ( const auto& light : m_lights )
			{
				if ( light != lightSceneInfo
					&& light->Proxy()->IsUsedAsAtmosphereSunLight() )
				{
					m_skyAtmosphereLight = light;
				}
			}
		}
	}

	bool UpdateGPUPrimitiveInfos( Scene& scene )
	{
		assert( IsInRenderThread() );
		uint32 updateSize = static_cast<uint32>( scene.m_primitiveToUpdate.size() );
		if ( updateSize == 0 )
		{
			return true;
		}

		DistributionCopyCS computeShader;
		ComputeShader* cs = computeShader.Shader();
		if ( cs == nullptr )
		{
			return false;
		}

		uint32 totalPrimitives = static_cast<uint32>( scene.m_primitives.Size() );
		scene.m_gpuPrimitiveInfos.Resize( totalPrimitives );

		GpuMemcpy gpuMemcpy( updateSize, sizeof( PrimitiveSceneData ) / sizeof( Vector4 ), scene.m_uploadPrimitiveBuffer, scene.m_distributionBuffer );

		for ( auto primitiveId : scene.m_primitiveToUpdate )
		{
			PrimitiveSceneData param( scene, primitiveId );

			gpuMemcpy.Add( (const char*)( &param ), primitiveId );
		}

		agl::Buffer* gpuPrimitiveInfos = scene.m_gpuPrimitiveInfos.Resource();
		gpuMemcpy.Upload( gpuPrimitiveInfos );

		scene.m_primitiveToUpdate.clear();
		return true;
	}
}
