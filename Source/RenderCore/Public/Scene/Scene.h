#pragma once

#include "DrawSnapshot.h"
#include "IScene.h"
#include "LightSceneInfo.h"
#include "PassProcessor.h"
#include "Physics/BoxSphereBounds.h"
#include "SceneConstantBuffers.h"
#include "SceneVelocityData.h"
#include "SizedTypes.h"
#include "UploadBuffer.h"

#include <set>

namespace rendercore
{
	class HemisphereLightProxy;
	class PrimitiveSceneInfo;
	class SkyAtmosphereRenderSceneInfo;
	class TexturedSkyProxy;
	class VolumetricCloudSceneInfo;
	class VolumetricFogSceneInfo;

	class Scene final : public IScene
	{
	public:
		virtual void AddPrimitive( PrimitiveComponent* primitive ) override;
		virtual void RemovePrimitive( PrimitiveComponent* primitive ) override;
		virtual void UpdatePrimitiveTransform( PrimitiveComponent* primitive ) override;
		virtual SparseArray<PrimitiveSceneInfo*>& Primitives() override
		{
			return m_primitives;
		}
		virtual const SparseArray<PrimitiveSceneInfo*>& Primitives() const override
		{
			return m_primitives;
		}

		virtual void AddTexturedSkyComponent( TexturedSkyComponent* texturedSky ) override;
		virtual void RemoveTexturedSkyComponent( TexturedSkyComponent* texturedSky ) override;

		virtual void AddSkyAtmosphere( SkyAtmospherePorxy* skyAtmosphereProxy ) override;
		virtual void RemoveAtomosphere( SkyAtmospherePorxy* skyAtmosphereProxy ) override;

		virtual void AddVolumetricCloud( VolumetricCloudComponent* volumetricCloud ) override;
		virtual void RemoveVolumetricCloud( VolumetricCloudComponent* volumetricCloud ) override;

		virtual void AddVolumetricFog( VolumetricFogComponent* volumetricFog ) override;
		virtual void RemoveVolumetricFog( VolumetricFogComponent* volumetricFog ) override;

		virtual void AddHemisphereLightComponent( HemisphereLightComponent* light ) override;
		virtual void RemoveHemisphereLightComponent( HemisphereLightComponent* light ) override;

		virtual void AddLight( LightComponent* light ) override;
		virtual void RemoveLight( LightComponent* light ) override;

		virtual SceneViewConstantBuffer& SceneViewConstant() override
		{
			return m_viewConstant;
		}
		virtual const SceneViewConstantBuffer& SceneViewConstant() const override
		{
			return m_viewConstant;
		}

		virtual ScenePrimitiveBuffer& GpuPrimitiveInfo() override
		{
			return m_gpuPrimitiveInfos;
		}

		virtual const ScenePrimitiveBuffer& GpuPrimitiveInfo() const override
		{
			return m_gpuPrimitiveInfos;
		}

		virtual ShadingMethod GetShadingMethod() const override;

		virtual Scene* GetRenderScene() override
		{ 
			return this; 
		};

		virtual void OnBeginSceneRendering() override;

		virtual uint64 GetNumFrame() const override
		{
			return m_internalNumFrame;
		}

		void AddPrimitiveToUpdate( uint32 primitiveId );

		const SparseArray<LightSceneInfo*>& Lights() const
		{
			return m_lights;
		}

		SparseArray<BoxSphereBounds>& PrimitiveBounds()
		{
			return m_primitiveBounds;
		}

		const SparseArray<BoxSphereBounds>& PrimitiveBounds() const
		{
			return m_primitiveBounds;
		}

		[[nodiscard]] CachedDrawSnapshotInfo AddCachedDrawSnapshot( RenderPass passType, const DrawSnapshot& snapshot );
		void RemoveCachedDrawSnapshot( const CachedDrawSnapshotInfo& info );
		SparseArray<DrawSnapshot>& CachedSnapshots( RenderPass passType ) { return m_cachedSnapshots[static_cast<uint32>( passType )]; }

		TexturedSkyProxy* TexturedSky()
		{
			return m_texturedSky;
		}

		const HemisphereLightProxy* HemisphereLight() const
		{
			return m_hemisphereLight;
		}

		SkyAtmosphereRenderSceneInfo* SkyAtmosphereSceneInfo()
		{
			return m_skyAtmosphere;
		}

		VolumetricCloudSceneInfo* VolumetricCloud()
		{
			return m_volumetricCloud;
		}

		VolumetricFogSceneInfo* VolumetricFog()
		{
			return m_volumetricFog;
		}

		const LightSceneInfo* SkyAtmosphereSunLight()
		{
			return m_skyAtmosphereLight;
		}

		std::optional<Matrix> GetPreviousTransform( uint32 primitiveId ) const;

	private:
		void AddPrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo );
		void RemovePrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo );

		void AddTexturedSky( TexturedSkyProxy* texturedSky );
		void RemoveTexturedSky( TexturedSkyProxy* texturedSky );

		void AddVolumetricCloud( VolumetricCloudSceneInfo* volumetricCloudSceneInfo );
		void RemoveVolumetricCloud( VolumetricCloudSceneInfo* volumetricCloudSceneInfo );

		void AddVolumetricFog( VolumetricFogSceneInfo* volumetricFogSceneInfo );
		void RemoveVolumetricFog( VolumetricFogSceneInfo* volumetricFogSceneInfo );

		void AddHemisphereLight( HemisphereLightProxy* hemisphereLight );
		void RemoveHemisphereLight( HemisphereLightProxy* hemisphereLight );

		void AddLightSceneInfo( LightSceneInfo* lightSceneInfo );
		void RemoveLightSceneInfo( LightSceneInfo* lightSceneInfo );

		void AddSkyAtmosphereLight( LightSceneInfo* lightSceneInfo );
		void RemoveSkyAtmosphereLight( LightSceneInfo* lightSceneInfo );

		SparseArray<PrimitiveSceneInfo*> m_primitives;
		SparseArray<BoxSphereBounds> m_primitiveBounds;

		TexturedSkyProxy* m_texturedSky = nullptr;

		SkyAtmosphereRenderSceneInfo* m_skyAtmosphere = nullptr;
		VolumetricCloudSceneInfo* m_volumetricCloud = nullptr;
		VolumetricFogSceneInfo* m_volumetricFog = nullptr;

		LightSceneInfo* m_skyAtmosphereLight = nullptr;

		HemisphereLightProxy* m_hemisphereLight = nullptr;

		SparseArray<LightSceneInfo*> m_lights;

		SparseArray<DrawSnapshot> m_cachedSnapshots[static_cast<uint32>( RenderPass::Count )];
		CachedDrawSnapshotBucket m_cachedSnapshotBuckect;

		SceneViewConstantBuffer m_viewConstant;

		std::set<uint32> m_primitiveToUpdate;
		ScenePrimitiveBuffer m_gpuPrimitiveInfos;

		TypedUploadBuffer<Vector4> m_uploadPrimitiveBuffer;
		TypedUploadBuffer<uint32> m_distributionBuffer;

		uint64 m_internalNumFrame = 0;

		SceneVelocityData m_velocityData;

		friend bool UpdateGPUPrimitiveInfos( Scene& scene );
	};

	bool UpdateGPUPrimitiveInfos( Scene& scene );
}
