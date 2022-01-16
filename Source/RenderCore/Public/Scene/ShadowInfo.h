#pragma once

#include "DrawSnapshot.h"
#include "GraphicsApiResource.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Physics/BoxSphereBounds.h"
#include "SizedTypes.h"
#include "Texture.h"
#include "TypedBuffer.h"

class LightSceneInfo;
class PrimitiveSceneInfo;
class RenderingShaderResource;
class SceneRenderer;

struct RenderView;

struct CascadeShadowSetting
{
	static constexpr uint32 MAX_CASCADE_NUM = 2;
	float m_splitDistance[MAX_CASCADE_NUM + 1] = {};
};

struct ShadowDepthPassParameters
{
	Vector4 m_lightPosOrDir;
	float m_slopeBiasScale;
	float m_constantBias;
	float m_padding1[2];

	Vector4 m_cascadeFar[CascadeShadowSetting::MAX_CASCADE_NUM];
	Matrix m_shadowViewProjection[6];
};

struct ShadowMapRenderTarget
{
	RefHandle<aga::Texture> m_shadowMap;
	RefHandle<aga::Texture> m_shadowMapDepth;
};

class ShadowInfo
{
public:
	bool HasCasterPrimitives() const
	{
		return m_shadowCasters.size() > 0;
	}

	ShadowMapRenderTarget& ShadowMap()
	{
		return m_shadowMap;
	}

	LightSceneInfo* GetLightSceneInfo()
	{
		return m_lightSceneInfo;
	}

	const RenderView* View() const
	{
		return m_view;
	}

	std::pair<uint32, uint32> ShadowMapSize() const
	{
		return { m_shadowMapWidth, m_shadowMapHeight };
	}

	const rendercore::VectorSingleFrame<BoxSphereBounds>& ShadowCastersViewSpaceBounds() const
	{
		return m_shadowCastersViewSpaceBounds;
	}

	const rendercore::VectorSingleFrame<BoxSphereBounds>& ShadowReceiversViewSpaceBounds() const
	{
		return m_shadowReceiversViewSpaceBounds;
	}

	float SubjectNear() const
	{
		return m_subjectNear;
	}

	float SubjectFar() const
	{
		return m_subjectFar;
	}

	Matrix( &ShadowViewProjections() )[6]
	{
		return m_shadowViewProjections;
	}

		CascadeShadowSetting& CascadeSetting()
	{
		return m_cacadeSetting;
	}

	TypedConstatBuffer<ShadowDepthPassParameters>& ConstantBuffer()
	{
		return m_shadowConstantBuffer;
	}

	void AddCasterPrimitive( PrimitiveSceneInfo* primitiveSceneInfo, const BoxSphereBounds& viewspaceBounds );
	void AddReceiverPrimitive( PrimitiveSceneInfo* primitiveSceneInfo, const BoxSphereBounds& viewspaceBounds );

	void SetupShadowConstantBuffer();
	void RenderDepth( SceneRenderer& renderer, RenderingShaderResource& resources );

	ShadowInfo( LightSceneInfo* lightSceneInfo, const RenderView& view );

private:
	void AddCachedDrawSnapshotForPass( PrimitiveSceneInfo& primitiveSceneInfo );
	void UpdateSubjectNearAndFar( const BoxSphereBounds& viewspaceBounds );

	LightSceneInfo* m_lightSceneInfo = nullptr;
	const RenderView* m_view = nullptr;
	ShadowMapRenderTarget m_shadowMap;
	uint32 m_shadowMapWidth = 0;
	uint32 m_shadowMapHeight = 0;

	rendercore::VectorSingleFrame<PrimitiveSceneInfo*> m_shadowCasters;
	rendercore::VectorSingleFrame<PrimitiveSceneInfo*> m_shadowReceivers;

	rendercore::VectorSingleFrame<BoxSphereBounds> m_shadowCastersViewSpaceBounds;
	rendercore::VectorSingleFrame<BoxSphereBounds> m_shadowReceiversViewSpaceBounds;

	float m_subjectNear = std::numeric_limits<float>::max();
	float m_subjectFar = std::numeric_limits<float>::min();

	rendercore::VectorSingleFrame<VisibleDrawSnapshot> m_snapshots;

	Matrix m_shadowViewProjections[6];

	CascadeShadowSetting m_cacadeSetting;

	TypedConstatBuffer<ShadowDepthPassParameters> m_shadowConstantBuffer;
};