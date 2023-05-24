#pragma once

#include "Components/Component.h"

namespace rendercore
{
	class VolumetricFogProxy;
}

class VolumetricFogComponent : public Component
{
	GENERATE_CLASS_TYPE_INFO( VolumetricFogComponent )

public:
	using Component::Component;

	virtual void LoadProperty( const JSON::Value& json ) override;

	rendercore::VolumetricFogProxy*& Proxy()
	{
		return m_proxy;
	}

	virtual rendercore::VolumetricFogProxy* CreateProxy();

protected:
	virtual bool ShouldCreateRenderState() const override;
	virtual void CreateRenderState() override;
	virtual void RemoveRenderState() override;

private:
	rendercore::VolumetricFogProxy* m_proxy = nullptr;
};