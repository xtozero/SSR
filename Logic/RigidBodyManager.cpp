#include "stdafx.h"
#include "Aaboundingbox.h"
#include "BoundingSphere.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/IMesh.h"
#include "RigidBodyManager.h"

#include <memory>

std::shared_ptr<IRigidBody> CRigidBodyManager::GetRigidBody( IRenderer& renderer, const String& meshName, RIGID_BODY_TYPE type )
{
	if ( type < 0 || type >= RIGID_BODY_TYPE::Count )
	{
		return nullptr;
	}

	auto found = m_rigidBodyList.find( meshName );

	if ( found == m_rigidBodyList.end( ) )
	{
		RigidBodyGroup newRigidBodyGroup;
		m_rigidBodyList.emplace( meshName, newRigidBodyGroup );
	}

	RigidBodyGroup& rigidBodyGroup = m_rigidBodyList[meshName];

	int rigidBodyType = static_cast<int>( type );

	if ( rigidBodyGroup.m_rigidBodies[rigidBodyType].get( ) == nullptr )
	{
		rigidBodyGroup.m_rigidBodies[rigidBodyType] = CreateRigidBody( renderer, meshName, type );
	}

	return rigidBodyGroup.m_rigidBodies[rigidBodyType];
}

std::shared_ptr<IRigidBody> CRigidBodyManager::CreateRigidBody( IRenderer& renderer, const String& meshName, RIGID_BODY_TYPE type )
{
	std::shared_ptr<IMesh> pMesh = renderer.GetModelPtr( meshName.c_str( ) );

	if ( pMesh.get( ) == nullptr )
	{
		return nullptr;
	}

	std::shared_ptr<IRigidBody> newRigidBody = nullptr;

	switch ( type )
	{
	case RIGID_BODY_TYPE::Sphere:
		newRigidBody = std::make_shared<BoundingSphere>( );
		break;
	case RIGID_BODY_TYPE::AABB:
		newRigidBody = std::make_shared<CAaboundingbox>( );
		break;
	case RIGID_BODY_TYPE::OBB:
		break;
	default:
		break;
	}

	if ( newRigidBody )
	{
		newRigidBody->CreateRigideBody( pMesh );
	}

	return newRigidBody;
}

std::unique_ptr<IRigidBody> CRigidBodyManager::MakeRigidBody( RIGID_BODY_TYPE type )
{
	switch ( type )
	{
	case RIGID_BODY_TYPE::Sphere:
		return std::make_unique<BoundingSphere>( );
		break;
	case RIGID_BODY_TYPE::AABB:
		return std::make_unique<CAaboundingbox>( );
		break;
	case RIGID_BODY_TYPE::OBB:
		break;
	default:
		break;
	}

	return nullptr;
}

CRigidBodyManager::CRigidBodyManager( )
{
}


CRigidBodyManager::~CRigidBodyManager( )
{
}
