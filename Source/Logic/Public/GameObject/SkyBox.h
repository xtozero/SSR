#pragma once

#include "GameObject.h"

class TexturedSkyComponent;

class CSkyBox : public CGameObject
{
	GENERATE_CLASS_TYPE_INFO( CSkyBox )

public:
	virtual bool IgnorePicking() const override { return true; }

	CSkyBox();
	~CSkyBox() = default;

private:
	TexturedSkyComponent* m_pTexturedSkyComponent = nullptr;
};
