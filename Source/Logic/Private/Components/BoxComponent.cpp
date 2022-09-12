#include "Components/BoxComponent.h"

#include "Physics/BodySetup.h"

namespace
{
	void UpdateSphereBodySetup( BodySetup& bodySetup, const Vector& halfSize )
	{
		std::vector<BoxElem>& boxElems = bodySetup.BoxElems();
		BoxElem lastElem = boxElems.back();

		lastElem.SetCenter( Vector::ZeroVector );
		lastElem.SetRotation( Quaternion::Identity );
		lastElem.SetHalfSize( halfSize );
	}
}

BoxSphereBounds BoxComponent::CalcBounds( const Matrix& transform )
{
	Vector points[] = {
		-m_halfSize,
		m_halfSize
	};

	return BoxSphereBounds( points, std::extent_v<decltype( points )> ).TransformBy( transform );
}

rendercore::PrimitiveProxy* BoxComponent::CreateProxy() const
{
	return nullptr;
}

void BoxComponent::UpdateBodySetup()
{
	CreateBodySetup<BoxElem>();
	UpdateSphereBodySetup( m_bodySetup.value(), m_halfSize );
}

void BoxComponent::SetHalfSize( const Vector& halfSize )
{
	m_halfSize = halfSize;

	UpdateBounds();
	UpdateBodySetup();
	MarkRenderStateDirty();

	if ( PhysicsStateCreated() )
	{
		// Ω∫ƒ…¿œ
	}
}
