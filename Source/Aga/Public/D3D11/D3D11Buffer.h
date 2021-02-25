#pragma once

#include "Buffer.h"
#include "D3D11ResourceInterface.h"

class D3D11BufferBase : public aga::Buffer
{
public:
	ID3D11Buffer* Buffer( ) { return m_buffer; }
	const ID3D11Buffer* Buffer( ) const { return m_buffer; }
	ID3D11ShaderResourceView* Srv( ) { return m_srv; }
	const ID3D11ShaderResourceView* Srv( ) const { return m_srv; }
	ID3D11UnorderedAccessView* Uav( ) { return m_uav; }
	const ID3D11UnorderedAccessView* Uav( ) const { return m_uav; }

	D3D11BufferBase( const BUFFER_TRAIT& trait, const void* initData );
	~D3D11BufferBase( );

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

	ID3D11ShaderResourceView* m_srv = nullptr;
	ID3D11UnorderedAccessView* m_uav = nullptr;
};
