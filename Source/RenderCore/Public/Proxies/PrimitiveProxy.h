#pragma once

class PrimitivieSceneInfo;

class PrimitiveProxy
{
public:
	virtual ~PrimitiveProxy( ) = default;

	PrimitivieSceneInfo* m_primitiveSceneInfo = nullptr;
};