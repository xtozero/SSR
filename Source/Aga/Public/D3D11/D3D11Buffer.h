#pragma once

#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "D3D11ResourceInterface.h"

class CD3D11Buffer : public GraphicsResourceBase<ID3D11Buffer>
{
public:
	virtual void InitResource( ) override;

	UINT Size( ) const { return m_desc.ByteWidth; }
	UINT Stride( ) const { return m_desc.StructureByteStride; }
	const D3D11_BUFFER_DESC& GetDesc( ) const { return m_desc; }

	CD3D11Buffer( const BUFFER_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	~CD3D11Buffer( )
	{
		delete m_dataStorage;
	}
	CD3D11Buffer( const CD3D11Buffer& ) = delete;
	CD3D11Buffer( CD3D11Buffer&& ) = default;
	CD3D11Buffer& operator=( const CD3D11Buffer& ) = delete;
	CD3D11Buffer& operator=( CD3D11Buffer&& ) = default;

private:
	D3D11_BUFFER_DESC m_desc;
	D3D11_SUBRESOURCE_DATA m_initData;
	void* m_dataStorage = nullptr;
};

class D3D11BufferBase
{
public:
	D3D11BufferBase( const BUFFER_TRAIT& trait, const void* initData );
	~D3D11BufferBase( );

protected:
	void CreateBuffer( );
	void DestroyBuffer( );

	ID3D11Buffer* m_buffer = nullptr;
	D3D11_BUFFER_DESC m_desc = {};
	D3D11_SUBRESOURCE_DATA m_initData = {};
	void* m_dataStorage = nullptr;
	bool m_hasInitData = false;
};

class D3D11ConstantBuffer : public ConstantBuffer, public D3D11BufferBase
{
public:
	virtual void InitResource( ) override;
	virtual void Free( ) override;

	D3D11ConstantBuffer( const BUFFER_TRAIT& trait );

private:
};

class D3D11VertexBuffer : public VertexBuffer, public D3D11BufferBase
{
public:
	virtual void InitResource( ) override;
	virtual void Free( ) override;

	D3D11VertexBuffer( const BUFFER_TRAIT& trait, const void* initData );
};

class D3D11IndexBuffer : public IndexBuffer, public D3D11BufferBase
{
public:
	virtual void InitResource( ) override;
	virtual void Free( ) override;

	D3D11IndexBuffer( const BUFFER_TRAIT& trait, const void* initData );
};