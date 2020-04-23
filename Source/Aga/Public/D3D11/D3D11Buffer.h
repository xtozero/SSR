#pragma once

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