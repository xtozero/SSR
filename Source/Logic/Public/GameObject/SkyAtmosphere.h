#pragma once

#include "GameObject.h"

class SkyAtmosphereComponent;

class SkyAtmosphere : public CGameObject
{
public:
	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;

	virtual bool IgnorePicking() const override { return true; }

	SkyAtmosphere();

private:
	SkyAtmosphereComponent* m_pSkyAtmosphereComponent = nullptr;
};