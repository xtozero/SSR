#pragma once

#include "GameObject.h"

class CSkyBox : public CGameObject
{
public:
	virtual void Think( float elapsedTime ) override;

	virtual bool IgnorePicking( ) const override { return true; }

	virtual bool ShouldDrawShadow( ) const override { return false; }

protected:
	virtual bool LoadModelMesh( CGameLogic& gameLogic ) override;

public:
	CSkyBox( ) = default;
	~CSkyBox( ) = default;
};
