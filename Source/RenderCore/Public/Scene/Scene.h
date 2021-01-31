#pragma once

#include "IScene.h"
#include "SceneConstantBuffers.h"

#include <vector>

class PrimitivieSceneInfo;

class Scene : public IScene
{
public:
	Scene( );

	virtual void AddPrimitive( PrimitiveComponent* primitive ) override;
	virtual void RemovePrimitive( PrimitiveComponent* primitive ) override;

	virtual void DrawScene( const RenderViewGroup& views ) override;

	virtual SHADING_METHOD ShadingMethod( ) const override;

private:
	void AddPrimitiveSceneInfo( PrimitivieSceneInfo* primitiveSceneInfo );
	void RemovePrimitiveSceneInfo( PrimitivieSceneInfo* primitiveSceneInfo );

	std::vector<PrimitivieSceneInfo*> m_primitives;
	SceneConstantBuffers m_constantBuffers;
};