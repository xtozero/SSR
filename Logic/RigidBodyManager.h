#pragma once

#include "common.h"
#include "IRigidBody.h"

#include <map>
#include <memory>

class IRenderer;

enum RIGID_BODY_TYPE
{
	Sphere = 0,
	AABB,
	OBB,
	Count,
};

struct RigidBodyGroup
{
	RigidBodyGroup( )
	{
		for ( int i = 0; i < RIGID_BODY_TYPE::Count; ++i )
		{
			m_rigidBodies[i] = nullptr;
		}
	}

	std::shared_ptr<IRigidBody> m_rigidBodies[RIGID_BODY_TYPE::Count];
};

class CRigidBodyManager
{
public:
	static CRigidBodyManager& GetInstance( )
	{
		static CRigidBodyManager instance;
		return instance;
	}

	std::shared_ptr<IRigidBody> GetRigidBody( IRenderer& renderer, const String& meshName, RIGID_BODY_TYPE type );
	std::shared_ptr<IRigidBody> CreateRigidBody( IRenderer& renderer, const String& meshName, RIGID_BODY_TYPE type );
	std::unique_ptr<IRigidBody> MakeRigidBody( RIGID_BODY_TYPE type );

private:
	std::map<String, RigidBodyGroup> m_rigidBodyList;

public:
	CRigidBodyManager( );
	~CRigidBodyManager( );
};

