#pragma once

#include "ShapeComponent.h"

namespace logic
{
	class SphereComponent : public ShapeComponent
	{
		GENERATE_CLASS_TYPE_INFO( SphereComponent )

	public:
		using ShapeComponent::ShapeComponent;

		virtual BoxSphereBounds CalcBounds( const Matrix& transform ) override;
		virtual rendercore::PrimitiveProxy* CreateProxy() const override;
		virtual void UpdateBodySetup() override;

		void SetRadius( float radius );

	private:
		float m_radius = 0.5;
	};
}
