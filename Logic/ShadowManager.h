#pragma once

class CLightManager;
class IBuffer;
class IRenderer;
class IRendererShadowManager;
class IMaterial;

class CShadowManager
{
public:
	void Init( IRenderer& renderer );
	void SceneBegin( CLightManager& lightMgr, IRenderer& renderer );
	void DrawScene( CLightManager& lightMgr, IRenderer& renderer, std::vector<std::shared_ptr<CGameObject>>& g_gameObjects );
	void SceneEnd( CLightManager& lightMgr, IRenderer& renderer );
	void Process( CLightManager& lightMgr, IRenderer& renderer, std::vector<std::shared_ptr<CGameObject>>& gameObjects );

	CShadowManager( );
	~CShadowManager( );

private:
	bool m_isEnabled;
	IRendererShadowManager* m_renderShadowMgr;
	IMaterial* m_shadowMapMtl;
};

