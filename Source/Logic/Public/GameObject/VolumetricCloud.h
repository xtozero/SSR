#pragma once

#include "GameObject.h"

class VolumetricCloudComponent;

class VolumetricCloud : public CGameObject
{
	GENERATE_CLASS_TYPE_INFO( VolumetricCloud )

public:
	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;
	VolumetricCloud();

private:
	VolumetricCloudComponent* m_volumetricCloudComponent = nullptr;
};