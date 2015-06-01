#pragma once

#include <D3D11.h>
#include "IBuffer.h"

class D3D11_CREATE_VB_TRAIT : public IBufferTrait
{
public:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	D3D11_BUFFER_DESC m_vbDesc;
	D3D11_PRIMITIVE_TOPOLOGY m_primTopology;
	D3D11_INPUT_ELEMENT_DESC* m_pInputElementDesc;
	UINT m_numInputElement;
	ID3D10Blob* m_pD3DBlob;
};

class D3D11_SET_VB_TRAIT : public IBufferTrait
{
public:
	ID3D11DeviceContext* m_pDeviceContext;
	UINT m_slot;
};

class D3D11VertexBuffer : public IBuffer
{
private:
	ID3D11Buffer* m_pVertexBuffer;
	D3D11_PRIMITIVE_TOPOLOGY m_primTopology;
	ID3D11InputLayout* m_inputLayout;
	UINT m_strides;
	UINT m_offset;

public:
	virtual bool CreateBuffer ( const D3D11_CREATE_VB_TRAIT& trait );
	virtual void SetBuffer ( const D3D11_SET_VB_TRAIT& trait );

	D3D11VertexBuffer ();
	~D3D11VertexBuffer ();
};

