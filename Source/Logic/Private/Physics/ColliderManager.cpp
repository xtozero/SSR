#include "stdafx.h"
#include "Physics/ColliderManager.h"

#include "Model/IMesh.h"
#include "Physics/Aaboundingbox.h"
#include "Physics/BoundingSphere.h"
#include "Physics/OrientedBoundingBox.h"
//#include "Render/IRenderer.h"

#include <memory>

class CColliderManager : public IColladerManager
{
public:
	virtual ICollider* GetCollider( const IMesh& mesh, COLLIDER::TYPE type ) override
	{
		ICollider* newCollider = CreateCollider( type );

		if ( newCollider )
		{
			newCollider->CalcMeshBounds( mesh );
		}

		return newCollider;
	}

	virtual Owner<ICollider*> CreateCollider( COLLIDER::TYPE type ) override
	{
		switch ( type )
		{
		case COLLIDER::SPHERE:
			return new BoundingSphere;
			break;
		case COLLIDER::AABB:
			return new CAaboundingbox;
			break;
		case COLLIDER::OBB:
			return new COrientedBoundingBox;
			break;
		default:
			break;
		}

		return nullptr;
	}
	

private:
	Owner<ICollider*> CreateCollider( const IMesh& mesh, COLLIDER::TYPE type )
	{
		if ( type < 0 || type >= COLLIDER::COUNT )
		{
			return nullptr;
		}

		const char* meshName = mesh.GetName( );
		auto found = m_colliderList.find( meshName );

		if ( found == m_colliderList.end( ) )
		{
			ColliderGroup newColliderGroup;
			m_colliderList.emplace( meshName, std::move( newColliderGroup ) );
		}

		ColliderGroup& colliderGroup = m_colliderList[meshName];

		int colliderType = static_cast<int>( type );

		if ( colliderGroup.m_colliders[colliderType].get( ) == nullptr )
		{
			colliderGroup.m_colliders[colliderType].reset( CreateCollider( mesh, type ) );
		}

		return colliderGroup.m_colliders[colliderType].get( );
	}

	std::map<std::string, ColliderGroup> m_colliderList;
};

IColladerManager& GetColliderManager( )
{
	static CColliderManager colliderManager;
	return colliderManager;
}
