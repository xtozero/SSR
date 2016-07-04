#pragma once

#include "IRendererShadowManager.h"

#include <memory>

class IDepthStencil;
class IRenderTarget;
class IShaderResource;
class ITexture;

class CRendererShadowManager : public IRendererShadowManager
{
public:
	CRendererShadowManager();
	virtual void SceneBegin( IRenderer* pRenderer ) override;
	virtual void SceneEnd( IRenderer* pRenderer ) override;

	virtual void CreateShadowMapTexture( IRenderer* pRenderer ) override;
private:
	ITexture* m_shadowMap;
	IShaderResource* m_srvShadowMap;
	IRenderTarget* m_rtvShadowMap;
	IDepthStencil* m_dsvShadowMap;

	bool m_isEnabled;
};

std::unique_ptr<IRendererShadowManager> CreateShadowManager();