#pragma once

class PrimitiveProxy;

class PrimitivieSceneInfo
{
public:
	PrimitivieSceneInfo( PrimitiveProxy* proxy ) : m_proxy( proxy ) {}

	PrimitiveProxy* m_proxy = nullptr;
};