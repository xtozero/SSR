#pragma once

#include "IScene.h"
#include "SceneConstantBuffers.h"
#include "UploadBuffer.h"

#include <vector>

class PrimitiveSceneInfo;

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

	virtual void DrawScene( const RenderViewGroup& views ) override;

	virtual SHADING_METHOD ShadingMethod( ) const override;

	virtual Scene* GetRenderScene( ) { return this; };

private:
	void AddPrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo );
	void RemovePrimitiveSceneInfo( PrimitiveSceneInfo* primitiveSceneInfo );

	std::vector<PrimitiveSceneInfo*> m_primitives;
	SceneViewConstantBuffer m_viewConstant;

	std::vector<std::size_t> m_primitiveToUpdate;
	ScenePrimitiveBuffer m_gpuPrimitiveInfos;

	TypedUploadBuffer<CXMFLOAT4> m_uploadPrimitiveBuffer;
	TypedUploadBuffer<UINT> m_distributionBuffer;

	friend bool UpdateGPUPrimitiveInfos( Scene& scene );
};

bool UpdateGPUPrimitiveInfos( Scene& scene );