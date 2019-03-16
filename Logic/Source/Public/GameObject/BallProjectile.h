#pragma once

#include "GameObject.h"

class CBallProjectile : public CGameObject
{
public:
	virtual void Think( float elapsedTime ) override;

	CBallProjectile( );
protected:
	virtual bool LoadModelMesh( CGameLogic& gameLogic ) override;
};
