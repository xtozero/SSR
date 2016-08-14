#include "stdafx.h"
#include "ITexture.h"
#include "TextureDescription.h"

#include <cassert>

TextureDescription::operator D3D11_TEXTURE1D_DESC( ) const
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_1D) );
	return m_textureDesc.m_texture1DDesc;
}

TextureDescription::operator D3D11_TEXTURE1D_DESC&()
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_1D) );
	return m_textureDesc.m_texture1DDesc;
}

TextureDescription::operator const D3D11_TEXTURE1D_DESC*() const
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_1D) );
	return &m_textureDesc.m_texture1DDesc;
}

TextureDescription::operator D3D11_TEXTURE2D_DESC( ) const
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_2D) );
	return m_textureDesc.m_texture2DDesc;
}

TextureDescription::operator D3D11_TEXTURE2D_DESC&()
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_2D) );
	return m_textureDesc.m_texture2DDesc;
}

TextureDescription::operator const D3D11_TEXTURE2D_DESC*() const
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_2D) );
	return &m_textureDesc.m_texture2DDesc;
}

TextureDescription::operator D3D11_TEXTURE3D_DESC( ) const
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_3D) );
	return m_textureDesc.m_texture3DDesc;
}

TextureDescription::operator const D3D11_TEXTURE3D_DESC*() const
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_3D) );
	return &m_textureDesc.m_texture3DDesc;
}

TextureDescription::operator D3D11_TEXTURE3D_DESC&()
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_3D) );
	return m_textureDesc.m_texture3DDesc;
}

TextureDescription& TextureDescription::operator=( D3D11_TEXTURE1D_DESC& desc )
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_1D) );
	m_textureDesc.m_texture1DDesc = desc;
	return *this;
}

TextureDescription& TextureDescription::operator=( D3D11_TEXTURE2D_DESC& desc )
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_2D) );
	m_textureDesc.m_texture2DDesc = desc;
	return *this;
}

TextureDescription& TextureDescription::operator=( D3D11_TEXTURE3D_DESC& desc )
{
	assert( m_textureType == static_cast<int>(TEXTURE_TYPE::TEXTURE_3D) );
	m_textureDesc.m_texture3DDesc = desc;
	return *this;
}

TextureDescription::TextureDescription( ) : m_textureType( static_cast<int>( TEXTURE_TYPE::TEXTURE_NONE ) )
{
	::ZeroMemory( &m_textureDesc, sizeof( m_textureDesc ) );
}

CShaderResourceViewDescription::operator D3D11_SHADER_RESOURCE_VIEW_DESC( ) const
{
	return m_srvDesc;
}

CShaderResourceViewDescription::operator D3D11_SHADER_RESOURCE_VIEW_DESC&()
{
	return m_srvDesc;
}

const D3D11_SHADER_RESOURCE_VIEW_DESC * CShaderResourceViewDescription::GetDesc( ) const
{
	return &m_srvDesc;
}

CShaderResourceViewDescription::CShaderResourceViewDescription( )
{
	::ZeroMemory( &m_srvDesc, sizeof( m_srvDesc ) );
}
