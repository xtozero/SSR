#pragma once

#include "INotifyGraphicsDevice.h"

#include <memory>
#include <vector>

class CGameLogic;
class CGameObject;
class CLightManager;
class IMaterial;
class IRenderer;
class IRenderResource;
class ITexture;

class CShadowManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	void Init( CGameLogic& gameLogic );
	void SceneBegin( CLightManager& lightMgr, CGameLogic& gameLogic );
	void DrawScene( CLightManager& lightMgr, CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	void SceneEnd( CLightManager& lightMgr, CGameLogic& gameLogic );
	void Process( CLightManager& lightMgr, CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );

private:
	bool CreateDeviceDependentResource( IRenderer& renderer );
	bool m_isEnabled = false;
	
	ITexture* m_shadowMap = nullptr;
	IRenderResource* m_srvShadowMap = nullptr;
	IRenderResource* m_rtvShadowMap = nullptr;
	IRenderResource* m_dsvShadowMap = nullptr;

	IMaterial* m_shadowMapMtl = nullptr;
};

