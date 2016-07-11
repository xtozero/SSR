#pragma once

#include "IMaterial.h"

struct ID3D11DeviceContext;

class IRenderState
{
public:
	virtual void Set( ID3D11DeviceContext*, const SHADER_TYPE type = SHADER_TYPE::NONE ) = 0;

	virtual ~IRenderState( ) = default;
};