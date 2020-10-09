#pragma once

#include "INotifyGraphicsDevice.h"
#include "Math/CXMFLOAT.h"
#include "Physics/Aaboundingbox.h"
//#include "Render/Resource.h"

#include <memory>
#include <vector>

class CGameLogic;
class CGameObject;
class CLightManager;
class IRenderer;
class IRenderResource;
class IResourceManager;
class ITexture;

struct RenderView;

class CShadowManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	void Init( CGameLogic& gameLogic );
	void BuildShadowProjectionMatrix( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	void DrawShadowMap( CGameLogic& gameLogic );
	void PrepareBeforeRenderScene( IRenderer& renderer );

private:
	bool CreateDeviceDependentResource( IRenderer& renderer );
	bool CreateNonCascadedResource( IResourceManager& resourceMgr );
	bool CreateCascadedResource( IResourceManager& resourceMgr );

	void DestroyShadowmapTexture( IResourceManager& resourceMgr );
	void DestroyNonCascadeResource( IResourceManager& resourceMgr );
	void DestoryCascadedResource( IResourceManager& resourceMgr );

	void ClassifyShadowCasterAndReceiver( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	void BuildOrthoShadowProjectionMatrix( CGameLogic& gameLogic, int cascadeLevel, float zClipNear, float zClipFar );
	void BuildPSMProjectionMatrix( CGameLogic& gameLogic, int cascadeLevel, float zClipNear, float zClipFar );
	void BuildLSPSMProjectionMatrix( CGameLogic& gameLogic, int cascadeLevel, float zClipNear, float zClipFar );

	bool m_isEnabled = false;
	bool m_isUnitClipCube = true;
	bool m_isSlideBack = true;
	bool m_useCascaded = false;
	
	//RE_HANDLE m_shadowMap = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_srvShadowMap = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_rtvShadowMap = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_shadowDepth = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_dsvShadowMap = RE_HANDLE::InValidHandle( );

	//RE_HANDLE m_vsShadowConstant = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_gsShadowConstant = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_psShadowConstant = RE_HANDLE::InValidHandle( );

	//RE_HANDLE m_shadowSampler = RE_HANDLE::InValidHandle( );

	//Material m_shadowMapMtl = INVALID_MATERIAL;

	std::vector<CGameObject*> m_shadowCasters;
	std::vector<CAaboundingbox> m_shadowCasterPoints;
	std::vector<CGameObject*> m_shadowReceivers;
	std::vector<CAaboundingbox> m_shadowReceiverPoints;

	float m_receiversNear = 0.f;
	float m_receiversFar = 0.f;
	float m_castersNear = 0.f;
	float m_castersFar = 0.f;
	float m_slideBack = 0.f;
	float m_minInfinityZ = 1.5f;
	float m_cosGamma = 0.f;
	float m_nOptWeight = 1.f;

	const static int MAX_CASCADED_NUM = 2;

	struct CascadeConstant
	{
		float m_zFar = 0.f;
		float m_padding[3];
	};

	struct VsNonCascadeConstant
	{
		float m_zBias = 0.f;
		float m_padding[3];
		CXMFLOAT4X4 m_lightView;
		CXMFLOAT4X4 m_lightProjection;
	};

	struct GsCascadeConstant
	{
		float m_zBias = 0.f;
		float m_padding[3];
		CXMFLOAT4X4 m_lightView[MAX_CASCADED_NUM];
		CXMFLOAT4X4 m_lightProjection[MAX_CASCADED_NUM];
	};

	struct PsCascadeConstant
	{
		CascadeConstant m_cascadeConstant[MAX_CASCADED_NUM];
		CXMFLOAT4X4 m_lightViewProjection[MAX_CASCADED_NUM];
	};

	float m_zBias = 1.f;
	CascadeConstant m_cascadeConstant[MAX_CASCADED_NUM];
	CXMFLOAT4X4 m_lightView[MAX_CASCADED_NUM];
	CXMFLOAT4X4 m_lightProjection[MAX_CASCADED_NUM];

	enum class ShadowType
	{
		ORTHO = 0,
		PSM,
		LSPSM,
	} m_shadowType = ShadowType::PSM;
};

