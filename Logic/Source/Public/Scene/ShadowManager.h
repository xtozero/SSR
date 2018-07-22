#pragma once

#include "INotifyGraphicsDevice.h"
#include "Math/CXMFLOAT.h"
#include "Physics/Aaboundingbox.h"
#include "Render/Resource.h"

#include <memory>
#include <vector>

class CGameLogic;
class CGameObject;
class CLightManager;
class CRenderView;
class IRenderer;
class IRenderResource;
class ITexture;

class CShadowManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	void Init( CGameLogic& gameLogic );
	void BuildShadowProjectionMatrix( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	void DrawShadowMap( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	const CXMFLOAT4X4& GetLightViewProjectionMatrix( ) const { return m_lightViewProjection; }

private:
	bool CreateDeviceDependentResource( IRenderer& renderer );
	void ClassifyShadowCasterAndReceiver( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	void BuildOrthoShadowProjectionMatrix( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	void BuildPSMProjectionMatrix( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	void BuildLSPSMProjectionMatrix( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );

	bool m_isEnabled = false;
	bool m_isUnitClipCube = true;
	bool m_isSlideBack = true;
	
	RE_HANDLE m_shadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_srvShadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_rtvShadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_shadowDepth = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_dsvShadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;

	RE_HANDLE m_shadowConstant = RE_HANDLE_TYPE::INVALID_HANDLE;

	RE_HANDLE m_shadowSampler = RE_HANDLE_TYPE::INVALID_HANDLE;

	Material m_shadowMapMtl = INVALID_MATERIAL;

	CXMFLOAT4X4 m_lightViewProjection;

	std::vector<CGameObject*> m_shadowCasters;
	std::vector<CAaboundingbox> m_shadowCasterPoints;
	std::vector<CGameObject*> m_shadowReceivers;
	std::vector<CAaboundingbox> m_shadowReceiverPoints;

	float m_zNear = 0.f;
	float m_zFar = 0.f;
	float m_slideBack = 0.f;
	float m_minInfinityZ = 1.5f;
	float m_cosGamma = 0.f;
	float m_nOptWeight = 1.f;

	struct ShadowConstant
	{
		float m_zBias = 0.001f;
		float padding[3];
	} m_constant;

	enum class ShadowType
	{
		Ortho = 0,
		PSM,
		LSPSM,
	} m_shadowType = ShadowType::LSPSM;
};

