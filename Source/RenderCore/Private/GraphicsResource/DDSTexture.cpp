#include "stdafx.h"
#include "DDSTexture.h"

Archive& operator<<( Archive& ar, TextureSection& section )
{
	ar << section.m_rowPitch << section.m_slicePitch << section.m_offset;
	return ar;
}

REGISTER_ASSET( Texture );
void Texture::Serialize( [[maybe_unused]] Archive& ar )
{
	// Do nothing
}

aga::Texture* Texture::Resource( )
{
	return m_texture.Get( );
}

const aga::Texture* Texture::Resource( ) const
{
	return m_texture.Get( );
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

	if ( ar.IsWriteMode( ) )
	{
		ar << m_sections.size( );
	}
	else
	{
		std::size_t size;
		ar << size;
		m_sections.resize( size );
	}

	for ( auto& section : m_sections )
	{
		ar << section;
	}
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

	m_sections = initializer.m_sections;
}

void DDSTexture::PostLoadImpl( )
{
	UINT misc = 0;
	misc |= m_isCubeMap ? RESOURCE_MISC::TEXTURECUBE : 0;
	misc |= ( m_depth > 1 ) ? RESOURCE_MISC::TEXTURE3D : 0;

	TEXTURE_TRAIT tarit =
	{
		m_width,
		m_height,
		m_depth * m_arraySize,
		1,
		0,
		m_mipLevels,
		m_format,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
		RESOURCE_BIND_TYPE::SHADER_RESOURCE,
		misc
	};

	RESOURCE_INIT_DATA initData;
	initData.m_sections.resize( m_sections.size( ) );
	initData.m_srcData = m_memory.Data( );
	initData.m_srcSize = m_memory.Size( );

	for ( std::size_t i = 0; i < initData.m_sections.size( ); ++i )
	{
		initData.m_sections[i].m_offset = m_sections[i].m_offset;
		initData.m_sections[i].m_pitch = m_sections[i].m_rowPitch;
		initData.m_sections[i].m_slicePitch = m_sections[i].m_slicePitch;
	}

	m_texture = aga::Texture::Create( tarit, &initData );

	EnqueueRenderTask( [this]( ) {
		m_texture->Init( );
	} );
}