#include "Components/SphereComponent.h"

#include "Physics/BodySetup.h"

namespace
{
	void UpdateSphereBodySetup( BodySetup& bodySetup, float sphereRadius )
	{
		std::vector<SphereElem>& sphereElems = bodySetup.SphereElems();
		SphereElem lastElem = sphereElems.back();

		lastElem.SetCenter( Vector::ZeroVector );
		lastElem.SetRadius( sphereRadius );
	}
}

BoxSphereBounds SphereComponent::CalcBounds( const Matrix& transform )
{
	return BoxSphereBounds( Vector::ZeroVector, Vector( m_radius, m_radius, m_radius ), m_radius ).TransformBy( transform );
}

PrimitiveProxy* SphereComponent::CreateProxy() const
{
	return nullptr;
}

void SphereComponent::UpdateBodySetup()
{
	CreateBodySetup<SphereElem>();
	UpdateSphereBodySetup( m_bodySetup.value(), m_radius );
}

void SphereComponent::SetRadius( float radius )
{
	m_radius = radius;

	UpdateBounds();
	UpdateBodySetup();
	MarkRenderStateDirty();

	if ( PhysicsStateCreated() )
	{
		// Ω∫ƒ…¿œ
	}
}
