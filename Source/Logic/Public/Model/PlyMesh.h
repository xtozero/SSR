#pragma once

#include "BaseMesh.h"

class CPlyMesh : public BaseMesh
{
public:
	virtual void Draw( CGameLogic& gameLogic ) override;
};

