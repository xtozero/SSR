#pragma once

#include "common.h"
#include "ICollider.h"
#include "Util.h"

#include <map>
#include <memory>

class IRenderer;

namespace COLLIDER
{
	enum TYPE
	{
		NONE = -1,
		SPHERE,
		AABB,
		OBB,
		COUNT,
	};
}

struct ColliderGroup
{
	std::unique_ptr<ICollider> m_colliders[COLLIDER::COUNT];
};

class IColladerManager
{
public:
	virtual ICollider* GetCollider( const IMesh& mesh, COLLIDER::TYPE type ) = 0;
	virtual Owner<ICollider*> CreateCollider( COLLIDER::TYPE type ) = 0;

	virtual ~IColladerManager( ) = default;
};

IColladerManager& GetColliderManager( );