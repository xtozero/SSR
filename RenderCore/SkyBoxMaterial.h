#pragma once

#include "Material.h"

class SkyBoxMaterial : public Material
{
public:
	virtual void Init( IRenderer& renderer ) override;
	virtual void SetShader( ) override;
};
