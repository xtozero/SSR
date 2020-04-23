#pragma once

#include "D3D11ResourceInterface.h"

class CD3D11Texture1D : public GraphicsResourceBase<ID3D11Texture1D>
{
public:
	virtual void InitResource( ) override;

	const D3D11_TEXTURE1D_DESC& GetDesc( ) const { return m_desc; }

	CD3D11Texture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	~CD3D11Texture1D( )
	{
		delete m_dataStorage;
	}
	CD3D11Texture1D( const CD3D11Texture1D& ) = delete;
	CD3D11Texture1D( CD3D11Texture1D&& ) = default;
	CD3D11Texture1D& operator=( const CD3D11Texture1D& ) = delete;
	CD3D11Texture1D& operator=( CD3D11Texture1D&& ) = delete;

private:
	D3D11_TEXTURE1D_DESC m_desc;
	D3D11_SUBRESOURCE_DATA m_initData;
	void* m_dataStorage = nullptr;
};

class CD3D11Texture2D : public GraphicsResourceBase<ID3D11Texture2D>, public FrameBufferDependent
{
public:
	virtual void InitResource( ) override;

	const D3D11_TEXTURE2D_DESC& GetDesc( ) const { return m_desc; }

	CD3D11Texture2D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	~CD3D11Texture2D( )
	{
		delete m_dataStorage;
	}
	CD3D11Texture2D( const CD3D11Texture2D& ) = delete;
	CD3D11Texture2D( CD3D11Texture2D&& ) = default;
	CD3D11Texture2D& operator=( const CD3D11Texture2D& ) = delete;
	CD3D11Texture2D& operator=( CD3D11Texture2D&& ) = delete;

private:
	D3D11_TEXTURE2D_DESC m_desc;
	D3D11_SUBRESOURCE_DATA m_initData;
	void* m_dataStorage = nullptr;
};

class CD3D11Texture3D : public GraphicsResourceBase<ID3D11Texture3D>
{
public:
	virtual void InitResource( ) override;

	const D3D11_TEXTURE3D_DESC& GetDesc( ) const { return m_desc; }

	CD3D11Texture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	~CD3D11Texture3D( )
	{
		delete m_dataStorage;
	}
	CD3D11Texture3D( const CD3D11Texture3D& ) = delete;
	CD3D11Texture3D( CD3D11Texture3D&& ) = default;
	CD3D11Texture3D& operator=( const CD3D11Texture3D& ) = delete;
	CD3D11Texture3D& operator=( CD3D11Texture3D&& ) = delete;

private:
	D3D11_TEXTURE3D_DESC m_desc;
	D3D11_SUBRESOURCE_DATA m_initData;
	void* m_dataStorage = nullptr;
};