#pragma once

#include "ShapeComponent.h"

namespace logic
{
	class BoxComponent : public ShapeComponent
	{
		GENERATE_CLASS_TYPE_INFO( BoxComponent )

	public:
		using ShapeComponent::ShapeComponent;

		virtual BoxSphereBounds CalcBounds( const Matrix& transform ) override;
		virtual rendercore::PrimitiveProxy* CreateProxy() const override;
		virtual void UpdateBodySetup() override;

		void SetHalfSize( const Vector& halfSize );

	private:
		Vector m_halfSize = Vector( 0.5f, 0.5f, 0.5f );
	};
}
