#pragma once

#include <D3D11.h>
#include "IBuffer.h"

class D3D11_CREATE_IB_TRAIT : public IBufferTrait
{
public:
	ID3D11Device* m_pDevice;
	D3D11_BUFFER_DESC m_ibDesc;
};

class D3D11IndexBuffer :
	public IBuffer
{
private:
	ID3D11Buffer* m_pIndexBuffer;
public:
	virtual bool CreateBuffer ( const D3D11_CREATE_IB_TRAIT& trait );

	D3D11IndexBuffer ();
	~D3D11IndexBuffer ();
};

