#pragma once

#include "INotifyGraphicsDevice.h"
#include "Render/Resource.h"

#include <memory>
#include <vector>

class CGameLogic;
class CGameObject;
class CLightManager;
class IRenderer;
class IRenderResource;
class ITexture;

class CShadowManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	void Init( CGameLogic& gameLogic );
	void Process( CLightManager& lightMgr, CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );

private:
	bool CreateDeviceDependentResource( IRenderer& renderer );
	bool m_isEnabled = false;
	
	RE_HANDLE m_shadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_srvShadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_rtvShadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_shadowDepth = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_dsvShadowMap = RE_HANDLE_TYPE::INVALID_HANDLE;

	Material m_shadowMapMtl = INVALID_MATERIAL;
};

