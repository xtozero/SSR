#pragma once

#include "DrawSnapshot.h"
#include "IScene.h"
#include "LightSceneInfo.h"
#include "SceneConstantBuffers.h"
#include "SizedTypes.h"
#include "SparseArray.h"
#include "UploadBuffer.h"

#include <vector>

class PrimitiveSceneInfo;
class TexturedSkyProxy;

class Scene final : public IScene
{
public:
	Scene( );

	virtual void AddPrimitive( PrimitiveComponent* primitive ) override;
	virtual void RemovePrimitive( PrimitiveComponent* primitive ) override;
	virtual std::vector<PrimitiveSceneInfo*>& Primitives( ) override
	{
		return m_primitives;
	}
	virtual const std::vector<PrimitiveSceneInfo*>& Primitives( ) const override
	{
		return m_primitives;
	}

	virtual void AddTexturedSkyComponent( TexturedSkyComponent* texturedSky ) override;
	virtual void RemoveTexturedSkyComponent( TexturedSkyComponent* texturedSky ) override;

	virtual void AddLight( LightComponent* light ) override;
	virtual void RemoveLight( LightComponent* light ) override;
	
	const SparseArray<LightSceneInfo*>& Lights( ) const
	{
		return m_lights;
	}

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

	[[nodiscard]] CachedDrawSnapshotInfo AddCachedDrawSnapshot( const DrawSnapshot& snapshot );
	void RemoveCachedDrawSnapshot( const CachedDrawSnapshotInfo& info );
	SparseArray<DrawSnapshot>& CachedSnapshots( ) { return m_cachedSnapshots; }
	 
	TexturedSkyProxy* TexturedSky( )
	{
		return m_texturedSky;
	}

private:
	void AddPrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo );
	void RemovePrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo );

	void AddTexturedSky( TexturedSkyProxy* texturedSky );
	void RemoveTexturedSky( TexturedSkyProxy* texturedSky );

	void AddLightSceneInfo( LightSceneInfo* lightSceneInfo );
	void RemoveLightSceneInfo( LightSceneInfo* lightSceneInfo );

	std::vector<PrimitiveSceneInfo*> m_primitives;
	SparseArray<LightSceneInfo*> m_lights;
	TexturedSkyProxy* m_texturedSky = nullptr;

	SparseArray<DrawSnapshot> m_cachedSnapshots;
	CachedDrawSnapshotBucket m_cachedSnapshotBuckect;

	SceneViewConstantBuffer m_viewConstant;

	std::vector<uint32> m_primitiveToUpdate;
	ScenePrimitiveBuffer m_gpuPrimitiveInfos;

	TypedUploadBuffer<CXMFLOAT4> m_uploadPrimitiveBuffer;
	TypedUploadBuffer<uint32> m_distributionBuffer;

	friend bool UpdateGPUPrimitiveInfos( Scene& scene );
};

bool UpdateGPUPrimitiveInfos( Scene& scene );