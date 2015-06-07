#pragma once

#include <D3D11.h>
#include "IBuffer.h"

class D3D11BaseBuffer : public IBuffer
{
protected:
	ID3D11Buffer* CreateBuffer ( ID3D11Device* m_pDevice, const D3D11_BUFFER_DESC* bufferDesc, const D3D11_SUBRESOURCE_DATA* initData );

public:
	D3D11BaseBuffer ( );
	virtual ~D3D11BaseBuffer ( );
};

