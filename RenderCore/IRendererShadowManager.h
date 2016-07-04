#pragma once

class IRenderer;

class IRendererShadowManager
{
public:
	virtual void SceneBegin( IRenderer* pRenderer ) = 0;
	virtual void SceneEnd( IRenderer* pRenderer ) = 0;
	virtual void CreateShadowMapTexture( IRenderer* pRenderer ) = 0;

	~IRendererShadowManager( ) = default;
};