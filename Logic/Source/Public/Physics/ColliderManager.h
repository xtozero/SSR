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

class CColliderManager
{
public:
	static CColliderManager& GetInstance( )
	{
		static CColliderManager instance;
		return instance;
	}

	ICollider* GetCollider( const IMesh& mesh, COLLIDER::TYPE type );
	Owner<ICollider*> CreateCollider( COLLIDER::TYPE type );

private:
	Owner<ICollider*> CreateCollider( const IMesh& mesh, COLLIDER::TYPE type );

	std::map<String, ColliderGroup> m_colliderList;
};

