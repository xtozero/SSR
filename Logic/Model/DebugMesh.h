#pragma once

#include "BaseMesh.h"
#include "../common.h"

class TriangleMesh : public BaseMesh
{
public:
	virtual bool Load( IRenderer& renderer, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) override;

	virtual void Draw( CGameLogic& gameLogic ) override;

	TriangleMesh( );
};

