#pragma once

#include "Material.h"

class TextureMaterial : public Material
{
public:
	virtual void Init( IRenderer& renderer ) override;
	virtual void SetShader( ) override;
};

