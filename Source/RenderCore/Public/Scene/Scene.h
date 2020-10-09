#pragma once

#include "IScene.h"
#include <vector>

class PrimitivieSceneInfo;

class Scene : public IScene
{
public:
	virtual void AddPrimitive( PrimitiveComponent* primitive ) override;
	virtual void RemovePrimitive( PrimitiveComponent* primitive ) override;

	virtual void DrawScene( const RenderViewGroup& views ) override;

private:
	void AddPrimitiveSceneInfo( PrimitivieSceneInfo* primitiveSceneInfo );
	void RemovePrimitiveSceneInfo( PrimitivieSceneInfo* primitiveSceneInfo );

	std::vector<PrimitivieSceneInfo*> m_primitives;
};