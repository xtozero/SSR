#pragma once

#include "DrawSnapshot.h"
#include "IScene.h"
#include "LightSceneInfo.h"
#include "PassProcessor.h"
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
		virtual logic::World* GetWorld() const override;

		virtual void AddPrimitive( logic::PrimitiveComponent* primitive ) override;
		virtual void RemovePrimitive( logic::PrimitiveComponent* primitive ) override;
		virtual void UpdatePrimitiveTransform( logic::PrimitiveComponent* primitive ) override;
		virtual SparseArray<PrimitiveSceneInfo*>& Primitives() override
		{
			return m_primitives;
		}
		virtual const SparseArray<PrimitiveSceneInfo*>& Primitives() const override
		{
			return m_primitives;
		}
		SparseArray<BoxSphereBounds>& PrimitiveBounds() override
		{
			return m_primitiveBounds;
		}
		const SparseArray<BoxSphereBounds>& PrimitiveBounds() const override
		{
			return m_primitiveBounds;
		}
		SparseArray<BoxSphereBounds>& PrimitiveOcclusionBounds() override
		{
			return m_primitiveOcclusionBounds;
		}
		const SparseArray<BoxSphereBounds>& PrimitiveOcclusionBounds() const override
		{
			return m_primitiveOcclusionBounds;
		}

		virtual void AddTexturedSkyComponent( logic::TexturedSkyComponent* texturedSky ) override;
		virtual void RemoveTexturedSkyComponent( logic::TexturedSkyComponent* texturedSky ) override;

		virtual void AddSkyAtmosphere( SkyAtmospherePorxy* skyAtmosphereProxy ) override;
		virtual void RemoveAtomosphere( SkyAtmospherePorxy* skyAtmosphereProxy ) override;

		virtual void AddVolumetricCloud( logic::VolumetricCloudComponent* volumetricCloud ) override;
		virtual void RemoveVolumetricCloud( logic::VolumetricCloudComponent* volumetricCloud ) override;

		virtual void AddVolumetricFog( logic::VolumetricFogComponent* volumetricFog ) override;
		virtual void RemoveVolumetricFog( logic::VolumetricFogComponent* volumetricFog ) override;

		virtual void AddHemisphereLightComponent( logic::HemisphereLightComponent* light ) override;
		virtual void RemoveHemisphereLightComponent( logic::HemisphereLightComponent* light ) override;

		virtual void AddLight( logic::LightComponent* light ) override;
		virtual void RemoveLight( logic::LightComponent* light ) override;

		virtual ShaderArguments& GetViewShaderArguments() override
		{
			return *m_viewShaderArguments.Get();
		}
		virtual const ShaderArguments& GetViewShaderArguments() const override
		{
			return *m_viewShaderArguments.Get();
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

		Scene() = default;
		explicit Scene( logic::World& world );

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

		logic::World* m_world = nullptr;

		SparseArray<PrimitiveSceneInfo*> m_primitives;
		SparseArray<BoxSphereBounds> m_primitiveBounds;
		SparseArray<BoxSphereBounds> m_primitiveOcclusionBounds;

		TexturedSkyProxy* m_texturedSky = nullptr;

		SkyAtmosphereRenderSceneInfo* m_skyAtmosphere = nullptr;
		VolumetricCloudSceneInfo* m_volumetricCloud = nullptr;
		VolumetricFogSceneInfo* m_volumetricFog = nullptr;

		LightSceneInfo* m_skyAtmosphereLight = nullptr;

		HemisphereLightProxy* m_hemisphereLight = nullptr;

		SparseArray<LightSceneInfo*> m_lights;

		SparseArray<DrawSnapshot> m_cachedSnapshots[static_cast<uint32>( RenderPass::Count )];
		CachedDrawSnapshotBucket m_cachedSnapshotBuckect;

		RefHandle<ShaderArguments> m_viewShaderArguments;

		std::set<uint32> m_primitiveToUpdate;
		ScenePrimitiveBuffer m_gpuPrimitiveInfos;

		TypedUploadBuffer<Vector4> m_uploadPrimitiveBuffer;
		TypedUploadBuffer<uint32> m_distributionBuffer;

		uint64 m_internalNumFrame = 0;

		SceneVelocityData m_velocityData;

		friend void UpdateGPUPrimitiveInfos( Scene& scene );
	};

	void UpdateGPUPrimitiveInfos( Scene& scene );
}
