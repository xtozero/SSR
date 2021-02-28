#pragma once
#include <cstddef>

class PrimitiveProxy;

class PrimitiveSceneInfo
{
public:
	PrimitiveSceneInfo( PrimitiveProxy* proxy ) : m_sceneProxy( proxy ) {}

	PrimitiveProxy* m_sceneProxy = nullptr;
	std::size_t m_id = 0;
};