#pragma once

#include "DrawSnapshot.h"
#include "IScene.h"
#include "LightSceneInfo.h"
#include "PassProcessor.h"
#include "Physics/BoxSphereBounds.h"
#include "SceneConstantBuffers.h"
#include "SizedTypes.h"
#include "UploadBuffer.h"

#include <vector>

class HemisphereLightProxy;
class PrimitiveSceneInfo;
class TexturedSkyProxy;

class Scene final : public IScene
{
public:
	virtual void AddPrimitive( PrimitiveComponent* primitive ) override;
	virtual void RemovePrimitive( PrimitiveComponent* primitive ) override;
	virtual SparseArray<PrimitiveSceneInfo*>& Primitives( ) override
	{
		return m_primitives;
	}
	virtual const SparseArray<PrimitiveSceneInfo*>& Primitives( ) const override
	{
		return m_primitives;
	}

	virtual void AddTexturedSkyComponent( TexturedSkyComponent* texturedSky ) override;
	virtual void RemoveTexturedSkyComponent( TexturedSkyComponent* texturedSky ) override;

	virtual void AddHemisphereLightComponent( HemisphereLightComponent* light ) override;
	virtual void RemoveHemisphereLightComponent( HemisphereLightComponent* light ) override;

	virtual void AddLight( LightComponent* light ) override;
	virtual void RemoveLight( LightComponent* light ) override;

	virtual SceneViewConstantBuffer& SceneViewConstant( ) override
	{
		return m_viewConstant;
	}
	virtual const SceneViewConstantBuffer& SceneViewConstant( ) const override
	{
		return m_viewConstant;
	}

	virtual ScenePrimitiveBuffer& GpuPrimitiveInfo( ) override
	{
		return m_gpuPrimitiveInfos;
	}

	virtual const ScenePrimitiveBuffer& GpuPrimitiveInfo( ) const override
	{
		return m_gpuPrimitiveInfos;
	}

	virtual SHADING_METHOD ShadingMethod( ) const override;

	virtual Scene* GetRenderScene( ) { return this; };

	const SparseArray<LightSceneInfo*>& Lights( ) const
	{
		return m_lights;
	}

	SparseArray<BoxSphereBounds>& PrimitiveBounds( )
	{
		return m_primitiveBounds;
	}

	const SparseArray<BoxSphereBounds>& PrimitiveBounds( ) const
	{
		return m_primitiveBounds;
	}

	[[nodiscard]] CachedDrawSnapshotInfo AddCachedDrawSnapshot( RenderPass passType, const DrawSnapshot& snapshot );
	void RemoveCachedDrawSnapshot( const CachedDrawSnapshotInfo& info );
	SparseArray<DrawSnapshot>& CachedSnapshots( RenderPass passType ) { return m_cachedSnapshots[static_cast<uint32>( passType )]; }
	 
	TexturedSkyProxy* TexturedSky( )
	{
		return m_texturedSky;
	}

	const HemisphereLightProxy* HemisphereLight( ) const
	{
		return m_hemisphereLight;
	}

private:
	void AddPrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo );
	void RemovePrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo );

	void AddTexturedSky( TexturedSkyProxy* texturedSky );
	void RemoveTexturedSky( TexturedSkyProxy* texturedSky );

	void AddHemisphereLight( HemisphereLightProxy* hemisphereLight );
	void RemoveHemisphereLight( HemisphereLightProxy* hemisphereLight );

	void AddLightSceneInfo( LightSceneInfo* lightSceneInfo );
	void RemoveLightSceneInfo( LightSceneInfo* lightSceneInfo );

	SparseArray<PrimitiveSceneInfo*> m_primitives;
	SparseArray<BoxSphereBounds> m_primitiveBounds;

	TexturedSkyProxy* m_texturedSky = nullptr;

	HemisphereLightProxy* m_hemisphereLight = nullptr;

	SparseArray<LightSceneInfo*> m_lights;

	SparseArray<DrawSnapshot> m_cachedSnapshots[static_cast<uint32>( RenderPass::Count )];
	CachedDrawSnapshotBucket m_cachedSnapshotBuckect;

	SceneViewConstantBuffer m_viewConstant;

	std::vector<uint32> m_primitiveToUpdate;
	ScenePrimitiveBuffer m_gpuPrimitiveInfos;

	TypedUploadBuffer<CXMFLOAT4> m_uploadPrimitiveBuffer;
	TypedUploadBuffer<uint32> m_distributionBuffer;

	friend bool UpdateGPUPrimitiveInfos( Scene& scene );
};

bool UpdateGPUPrimitiveInfos( Scene& scene );