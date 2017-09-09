#pragma once

#include "common.h"
#include "IRigidBody.h"

#include "../shared/Util.h"

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
	std::unique_ptr<IRigidBody> m_rigidBodies[RIGID_BODY_TYPE::Count];
};

class CRigidBodyManager
{
public:
	static CRigidBodyManager& GetInstance( )
	{
		static CRigidBodyManager instance;
		return instance;
	}

	IRigidBody* GetRigidBody( IRenderer& renderer, const String& meshName, RIGID_BODY_TYPE type );
	Owner<IRigidBody*> MakeRigidBody( RIGID_BODY_TYPE type );

private:
	Owner<IRigidBody*> CreateRigidBody( IRenderer& renderer, const String& meshName, RIGID_BODY_TYPE type );

	std::map<String, RigidBodyGroup> m_rigidBodyList;
};

