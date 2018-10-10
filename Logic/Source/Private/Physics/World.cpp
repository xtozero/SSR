#include "stdafx.h"
#include "Physics/World.h"

#include "Physics/Body.h"

using namespace DirectX;

void World::StartFrame( )
{
	for ( auto body : m_bodies )
	{
		body->ClearAccumulators( );
		body->CalculateDerivedData( );
	}
}

void World::RunPhysics( float duration )
{
	m_registry.UpdateForces( duration );
	Integrate( duration );
}

void World::Integrate( float duration )
{
	for ( auto body : m_bodies )
	{
		body->Integrate( duration );
	}
}
