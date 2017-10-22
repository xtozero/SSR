#include "stdafx.h"
#include "RigidBodyManager.h"

#include "Aaboundingbox.h"
#include "BoundingSphere.h"
#include "Model/IMesh.h"

#include "../RenderCore/CommonRenderer/IRenderer.h"

#include <memory>

IRigidBody* CRigidBodyManager::GetRigidBody( const IMesh& mesh, RIGID_BODY_TYPE type )
{
	if ( type < 0 || type >= RIGID_BODY_TYPE::Count )
	{
		return nullptr;
	}

	const TCHAR* meshName = mesh.GetName( );
	auto found = m_rigidBodyList.find( meshName );

	if ( found == m_rigidBodyList.end( ) )
	{
		RigidBodyGroup newRigidBodyGroup;
		m_rigidBodyList.emplace( meshName, std::move( newRigidBodyGroup ) );
	}

	RigidBodyGroup& rigidBodyGroup = m_rigidBodyList[meshName];

	int rigidBodyType = static_cast<int>( type );

	if ( rigidBodyGroup.m_rigidBodies[rigidBodyType].get( ) == nullptr )
	{
		rigidBodyGroup.m_rigidBodies[rigidBodyType].reset( CreateRigidBody( mesh, type ) );
	}

	return rigidBodyGroup.m_rigidBodies[rigidBodyType].get();
}

Owner<IRigidBody*> CRigidBodyManager::CreateRigidBody( const IMesh& mesh, RIGID_BODY_TYPE type )
{
	IRigidBody* newRigidBody = nullptr;

	switch ( type )
	{
	case RIGID_BODY_TYPE::Sphere:
		newRigidBody = new BoundingSphere;
		break;
	case RIGID_BODY_TYPE::AABB:
		newRigidBody = new CAaboundingbox;
		break;
	case RIGID_BODY_TYPE::OBB:
		break;
	default:
		break;
	}

	if ( newRigidBody )
	{
		newRigidBody->CreateRigideBody( mesh );
	}

	return newRigidBody;
}

Owner<IRigidBody*> CRigidBodyManager::MakeRigidBody( RIGID_BODY_TYPE type )
{
	switch ( type )
	{
	case RIGID_BODY_TYPE::Sphere:
		return new BoundingSphere;
		break;
	case RIGID_BODY_TYPE::AABB:
		return new CAaboundingbox;
		break;
	case RIGID_BODY_TYPE::OBB:
		break;
	default:
		break;
	}

	return nullptr;
}
