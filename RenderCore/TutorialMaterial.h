#pragma once

#include "Material.h"

class TutorialMaterial : public Material
{
public:
	virtual void Init( IRenderer* pRenderer ) override;
};