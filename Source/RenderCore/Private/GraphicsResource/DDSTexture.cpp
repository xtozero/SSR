#include "stdafx.h"
#include "DDSTexture.h"

REGISTER_ASSET( Texture );
void Texture::Serialize( Archive& ar )
{
	// Do nothing
}

void Texture::PostLoadImpl( )
{
	// Do nothing
}

REGISTER_ASSET( DDSTexture );
void DDSTexture::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ar << m_width;
	ar << m_height;
	ar << m_depth;
	ar << m_arraySize;
	ar << m_mipLevels;

	ar << m_isCubeMap;
	ar << m_demension;

	ar << m_format;

	ar << m_memory;
}

DDSTexture::DDSTexture( const DDSTextureInitializer& initializer )
{
	m_width = initializer.m_width;
	m_height = initializer.m_height;
	m_depth = initializer.m_depth;
	m_arraySize = initializer.m_arraySize;
	m_mipLevels = initializer.m_mipLevels;

	m_isCubeMap = initializer.m_isCubeMap;
	m_demension = initializer.m_demension;

	m_format = initializer.m_format;

	new ( &m_memory ) BinaryChunk( initializer.m_size );

	std::memcpy( m_memory.Data( ), initializer.m_memory, initializer.m_size );
}

void DDSTexture::PostLoadImpl( )
{

}