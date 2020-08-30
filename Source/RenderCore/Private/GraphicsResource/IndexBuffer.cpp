#include "stdafx.h"
#include "GraphicsResource/IndexBuffer.h"

#include <numeric>

void IndexBuffer::Initialize( const std::vector<std::size_t>& rawData )
{
	if ( rawData.size( ) > std::numeric_limits<unsigned short>::max( ) )
	{
		m_stride = 4;
		m_rawData = new unsigned short[rawData.size( )];

		for ( std::size_t i = 0; i < rawData.size( ); ++i )
		{
			static_cast<unsigned short*>( m_rawData )[i] = static_cast<unsigned short>( rawData[i] );
		}
	}
	else
	{
		m_stride = 2;
		m_rawData = new unsigned int[rawData.size( )];

		for ( std::size_t i = 0; i < rawData.size( ); ++i )
		{
			static_cast<unsigned int*>( m_rawData )[i] = static_cast<unsigned int>( rawData[i] );
		}
	}
}

IndexBuffer::~IndexBuffer( )
{
	delete[] m_rawData;
}
