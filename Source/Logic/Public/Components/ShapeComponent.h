#pragma once

#include "Physics/BodySetup.h"
#include "PrimitiveComponent.h"

#include <optional>

class ShapeComponent : public PrimitiveComponent
{
	GENERATE_CLASS_TYPE_INFO( ShapeComponent )

public:
	using PrimitiveComponent::PrimitiveComponent;

	virtual PrimitiveProxy* CreateProxy() const override;
	virtual BodySetup* GetBodySetup() override;
	virtual void UpdateBodySetup();

protected:
	template <typename T>
	void CreateBodySetup();

	std::optional<BodySetup> m_bodySetup;
};