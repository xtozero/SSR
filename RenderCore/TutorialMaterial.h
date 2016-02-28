#pragma once

#include "Material.h"

class TutorialMaterial : public Material<TutorialMaterial>
{
public:
	virtual void Init( ) override;
};