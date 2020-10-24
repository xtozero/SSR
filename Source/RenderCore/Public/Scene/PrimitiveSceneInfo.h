#pragma once

class PrimitiveProxy;

class PrimitivieSceneInfo
{
public:
	PrimitivieSceneInfo( PrimitiveProxy* proxy ) : m_sceneProxy( proxy ) {}

	PrimitiveProxy* m_sceneProxy = nullptr;
};