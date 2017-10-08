#pragma once

class CLightManager;
class IBuffer;
class IMaterial;
class IRenderer;
class IRenderResource;
class ITexture;

class CShadowManager
{
public:
	void Init( IRenderer& renderer );
	void SceneBegin( CLightManager& lightMgr, IRenderer& renderer );
	void DrawScene( CLightManager& lightMgr, IRenderer& renderer, std::vector<std::unique_ptr<CGameObject>>& gameObjects );
	void SceneEnd( CLightManager& lightMgr, IRenderer& renderer );
	void Process( CLightManager& lightMgr, IRenderer& renderer, std::vector<std::unique_ptr<CGameObject>>& gameObjects );

private:
	bool m_isEnabled = false;
	
	ITexture* m_shadowMap = nullptr;
	IRenderResource* m_srvShadowMap = nullptr;
	IRenderResource* m_rtvShadowMap = nullptr;
	IRenderResource* m_dsvShadowMap = nullptr;

	IMaterial* m_shadowMapMtl = nullptr;
	IBuffer* m_cbShadow = nullptr;
};

