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
	const CXMFLOAT4X4& GetLightViewProjectionMatrix( ) const { return m_lightViewPorjection; }

private:
	bool CreateDeviceDependentResource( IRenderer& renderer );
	void ClassifyShadowCasterAndReceiver( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	void BuildOrthoShadowProjectionMatrix( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );

	bool m_isEnabled = false;
	bool m_isUnitClipCube = true;
	
	RE_HANDLE m_shadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_srvShadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_rtvShadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_shadowDepth = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_dsvShadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;

	Material m_shadowMapMtl = INVALID_MATERIAL;

	CXMFLOAT4X4 m_lightViewPorjection;

	std::vector<CGameObject*> m_shadowCaster;
	std::vector<CAaboundingbox> m_shadowCasterPoint;
	std::vector<CGameObject*> m_shadowReceiver;
	std::vector<CAaboundingbox> m_shadowReceiverPoint;
};

