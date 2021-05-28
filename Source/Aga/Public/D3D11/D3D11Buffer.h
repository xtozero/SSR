#pragma once

#include "Buffer.h"

#include <d3d11.h>

class D3D11BufferBase : public aga::Buffer
{
public:
	ID3D11Buffer* Resource( );
	const ID3D11Buffer* Resource( ) const;

	UINT Stride( ) const;

	D3D11BufferBase( const BUFFER_TRAIT& trait, const void* initData );
	~D3D11BufferBase( );
	D3D11BufferBase( const D3D11BufferBase& ) = delete;
	D3D11BufferBase& operator=( const D3D11BufferBase& ) = delete;
	D3D11BufferBase( D3D11BufferBase&& ) = delete;
	D3D11BufferBase& operator=( D3D11BufferBase&& ) = delete;

protected:
	virtual void InitResource( ) override;
	virtual void FreeResource( ) override;

	void CreateBuffer( );
	void DestroyBuffer( );

	ID3D11Buffer* m_buffer = nullptr;
	D3D11_BUFFER_DESC m_desc = {};
	D3D11_SUBRESOURCE_DATA m_initData = {};
	void* m_dataStorage = nullptr;
	bool m_hasInitData = false;
};
