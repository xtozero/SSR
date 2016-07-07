#pragma once

struct ID3D11DeviceContext;

class IRenderState
{
public:
	virtual void Set( ID3D11DeviceContext* ) = 0;

	virtual ~IRenderState( ) = default;
};