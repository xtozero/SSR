#pragma once

#include "SceneComponent.h"

namespace rendercore
{
	class SkyAtmospherePorxy;
}

namespace logic
{
	class SkyAtmosphereComponent : public SceneComponent
	{
		GENERATE_CLASS_TYPE_INFO( SkyAtmosphereComponent )

	public:
		using SceneComponent::SceneComponent;

	protected:
		virtual bool ShouldCreateRenderState() const override;
		virtual void CreateRenderState() override;
		virtual void RemoveRenderState() override;

	private:
		rendercore::SkyAtmospherePorxy* m_skyAtmosphereProxy = nullptr;
	};
}
