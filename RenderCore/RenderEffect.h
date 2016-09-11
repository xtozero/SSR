#pragma once

#include <memory>
#include <vector>

class IRenderer;

class IRenderEffect
{
public:
	virtual void SceneBegin( IRenderer& ) {};
	virtual void SceneEnd( IRenderer& ) {};

	virtual ~IRenderEffect( ) = default;
};

class CRenderEffect
{
public:
	void SceneBegin( IRenderer& renderer );
	void SceneEnd( IRenderer& renderer );

	CRenderEffect( );
private:
	std::vector<std::unique_ptr<IRenderEffect>> m_renderEffects;
};
