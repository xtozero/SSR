#pragma once
#include "Math/CXMFloat.h"

#include <vector>

class DrawSnapshot;
class PrimitiveSceneInfo;
class Scene;

class PrimitiveProxy
{
public:
	virtual ~PrimitiveProxy( ) = default;

	void SetTransform( const CXMFLOAT4X4& worldTransform );
	const CXMFLOAT4X4& GetTransform( ) const;

	virtual void TakeSnapshot( std::vector<DrawSnapshot>& snapshots ) = 0;

private:
	friend Scene;

	PrimitiveSceneInfo* m_primitiveSceneInfo = nullptr;
	CXMFLOAT4X4 m_worldTransform;
};