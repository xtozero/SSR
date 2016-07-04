#pragma once

#include "GameObject.h"

class CSkyBox : public CGameObject
{
public:
	virtual void Think( ) override;

	virtual bool IgnorePicking( ) const override { return true; }

	virtual bool ShouldDrawShadow( ) const override { return false; }

protected:
	virtual bool LoadModelMesh( ) override;

public:
	CSkyBox( ) = default;
	~CSkyBox( ) = default;
};
