#pragma once

#include "common.h"
#include <d3d11.h>
#include "IBuffer.h"

class RENDERCORE_DLL BaseMesh
{
protected:
	void* m_pModelData;

	IBuffer* m_pVertexBuffer;
	IBuffer* m_pIndexBuffer;

	D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;

public:
	virtual bool Load( );

protected:
	BaseMesh( );

public:
	virtual ~BaseMesh( );
};

