#pragma once

#include "SceneComponent.h"

class SkyAtmospherePorxy;

class SkyAtmosphereComponent : public SceneComponent
{
public:
	using SceneComponent::SceneComponent;

protected:
	virtual bool ShouldCreateRenderState() const override;
	virtual void CreateRenderState() override;
	virtual void RemoveRenderState() override;

private:
	SkyAtmospherePorxy* m_skyAtmosphereProxy = nullptr;
};