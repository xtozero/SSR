#include "Components/ShapeComponent.h"

PrimitiveProxy* ShapeComponent::CreateProxy() const
{
	assert( false && "Derived class needs to Implement this" );
	return nullptr;
}

BodySetup* ShapeComponent::GetBodySetup()
{
	UpdateBodySetup();
	return m_bodySetup ? &m_bodySetup.value() : nullptr;
}

void ShapeComponent::UpdateBodySetup()
{
	assert( false && "Derived class needs to Implement this" );
}

template <typename T>
void ShapeComponent::CreateBodySetup()
{
	if ( m_bodySetup.has_value() == false )
	{
		m_bodySetup = BodySetup();

		m_bodySetup->AddShapeToGeometry( T() );
	}
}

template void ShapeComponent::CreateBodySetup<SphereElem>();
template void ShapeComponent::CreateBodySetup<BoxElem>();
