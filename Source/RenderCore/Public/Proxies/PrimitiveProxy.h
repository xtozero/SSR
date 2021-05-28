#pragma once
#include "Math/CXMFloat.h"

#include <vector>

class DrawSnapshot;
class PrimitiveSceneInfo;
class Scene;

class PrimitiveProxy
{
public:
	void SetTransform( const CXMFLOAT4X4& worldTransform );
	const CXMFLOAT4X4& GetTransform( ) const;

	virtual void CreateRenderData( ) = 0;
	virtual void TakeSnapshot( std::vector<DrawSnapshot>& snapshots ) const = 0;

	virtual ~PrimitiveProxy( ) = default;

	std::size_t GetId( ) const;

private:
	friend Scene;

	PrimitiveSceneInfo* m_primitiveSceneInfo = nullptr;
	CXMFLOAT4X4 m_worldTransform;
};