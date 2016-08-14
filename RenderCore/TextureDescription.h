#pragma once

#include <D3D11.h>

union TextureDescUnion
{
	D3D11_TEXTURE1D_DESC m_texture1DDesc;
	D3D11_TEXTURE2D_DESC m_texture2DDesc;
	D3D11_TEXTURE3D_DESC m_texture3DDesc;
};

class TextureDescription
{
public:
	int GetType( ) const { return m_textureType; };
	void SetType( int type ) { m_textureType = type; }

	operator D3D11_TEXTURE1D_DESC( ) const;
	explicit operator D3D11_TEXTURE1D_DESC&( );
	operator const D3D11_TEXTURE1D_DESC*() const;
	operator D3D11_TEXTURE2D_DESC( ) const;
	explicit operator D3D11_TEXTURE2D_DESC&( );
	operator const D3D11_TEXTURE2D_DESC*() const;
	operator D3D11_TEXTURE3D_DESC( ) const;
	operator const D3D11_TEXTURE3D_DESC*() const;
	explicit operator D3D11_TEXTURE3D_DESC&( );

	TextureDescription& operator=( D3D11_TEXTURE1D_DESC& desc );
	TextureDescription& operator=( D3D11_TEXTURE2D_DESC& desc );
	TextureDescription& operator=( D3D11_TEXTURE3D_DESC& desc );

	TextureDescription( );
private:
	int m_textureType;
	TextureDescUnion m_textureDesc;
};

class CShaderResourceViewDescription
{
public:
	operator D3D11_SHADER_RESOURCE_VIEW_DESC( ) const;
	explicit operator D3D11_SHADER_RESOURCE_VIEW_DESC&( );

	const D3D11_SHADER_RESOURCE_VIEW_DESC* GetDesc( ) const;

	CShaderResourceViewDescription( );
private:
	D3D11_SHADER_RESOURCE_VIEW_DESC m_srvDesc;
};