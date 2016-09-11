#pragma once

class IRenderer;

class IRendererShadowManager
{
public:
	virtual void SceneBegin( IRenderer& renderer ) = 0;
	virtual void SceneEnd( IRenderer& renderer ) = 0;
	virtual void CreateShadowMapTexture( IRenderer& renderer ) = 0;

	~IRendererShadowManager( ) = default;
};