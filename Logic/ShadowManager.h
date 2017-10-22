#pragma once

class CGameLogic;
class CLightManager;
class IBuffer;
class IMaterial;
class IRenderResource;
class ITexture;

class CShadowManager
{
public:
	void Init( CGameLogic& gameLogic );
	void SceneBegin( CLightManager& lightMgr, CGameLogic& gameLogic );
	void DrawScene( CLightManager& lightMgr, CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	void SceneEnd( CLightManager& lightMgr, CGameLogic& gameLogic );
	void Process( CLightManager& lightMgr, CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects );

private:
	bool m_isEnabled = false;
	
	ITexture* m_shadowMap = nullptr;
	IRenderResource* m_srvShadowMap = nullptr;
	IRenderResource* m_rtvShadowMap = nullptr;
	IRenderResource* m_dsvShadowMap = nullptr;

	IMaterial* m_shadowMapMtl = nullptr;
	IBuffer* m_cbShadow = nullptr;
};

