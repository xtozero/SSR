#pragma once

#include "Material.h"

class TextureMaterial : public Material
{
public:
	virtual void Init( IRenderer* pRenderer ) override;
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) override;
};

