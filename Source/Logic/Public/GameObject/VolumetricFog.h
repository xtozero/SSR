#pragma once

#include "GameObject.h"

class VolumetricFogComponent;

class VolumetricFog : public CGameObject
{
	GENERATE_CLASS_TYPE_INFO( VolumetricFog )

public:
	VolumetricFog();

private:
	VolumetricFogComponent* m_volumetricFogComponent = nullptr;
};