#include "stdafx.h"
#include "Physics/ColliderManager.h"

#include "Model/IMesh.h"
#include "Physics/Aaboundingbox.h"
#include "Physics/BoundingSphere.h"
#include "Render/IRenderer.h"

#include <memory>

ICollider* CColliderManager::GetCollider( const IMesh& mesh, COLLIDER::TYPE type )
{
	if ( type < 0 || type >= COLLIDER::COUNT )
	{
		return nullptr;
	}

	const TCHAR* meshName = mesh.GetName( );
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

	return colliderGroup.m_colliders[colliderType].get();
}

Owner<ICollider*> CColliderManager::CreateCollider( const IMesh& mesh, COLLIDER::TYPE type )
{
	ICollider* newCollider = nullptr;

	switch ( type )
	{
	case COLLIDER::SPHERE:
		newCollider = new BoundingSphere;
		break;
	case COLLIDER::AABB:
		newCollider = new CAaboundingbox;
		break;
	case COLLIDER::OBB:
		break;
	default:
		break;
	}

	if ( newCollider )
	{
		newCollider->CalcMeshBounds( mesh );
	}

	return newCollider;
}

Owner<ICollider*> CColliderManager::CreateCollider( COLLIDER::TYPE type )
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
		break;
	default:
		break;
	}

	return nullptr;
}
