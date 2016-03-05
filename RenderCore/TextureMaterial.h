#pragma once

#include "Material.h"

class TextureMaterial : public Material
{
public:
	virtual void Init( ) override;
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) override;
};

