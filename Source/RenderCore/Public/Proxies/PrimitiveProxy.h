#pragma once
#include "Math/CXMFloat.h"

class PrimitivieSceneInfo;
class Scene;

class PrimitiveProxy
{
public:
	virtual ~PrimitiveProxy( ) = default;

	void SetTransform( const CXMFLOAT4X4& worldTransform );
	const CXMFLOAT4X4& GetTransform( ) const;

private:
	friend Scene;

	PrimitivieSceneInfo* m_primitiveSceneInfo = nullptr;
	CXMFLOAT4X4 m_worldTransform;
};