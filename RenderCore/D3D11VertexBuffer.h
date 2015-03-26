#pragma once

#include <D3D11.h>
#include "IBuffer.h"

class D3D11_CREATE_VB_TRAIT : public IBufferTrait
{
public:
	ID3D11Device* m_pDevice;
	D3D11_BUFFER_DESC m_vbDesc;
};

class D3D11VertexBuffer : public IBuffer
{
private:
	ID3D11Buffer* m_pVertexBuffer;

public:
	virtual bool CreateBuffer ( const D3D11_CREATE_VB_TRAIT& trait );

	D3D11VertexBuffer ();
	~D3D11VertexBuffer ();
};

