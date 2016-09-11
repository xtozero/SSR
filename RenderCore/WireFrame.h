#pragma once
#include "Material.h"

class WireFrame : public Material
{
public:
	virtual void Init( IRenderer& renderer ) override;
};