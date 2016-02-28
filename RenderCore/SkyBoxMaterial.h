#pragma once

#include "Material.h"

class SkyBoxMaterial : public Material<SkyBoxMaterial>
{
public:
	virtual void Init( ) override;
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) override;
};
