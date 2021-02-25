#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"

#include <cstddef>

class UploadBuffer
{
public:
	static UploadBuffer Create( std::size_t elementSize, std::size_t numElement, const void* initData );
	void Resize( std::size_t size );
	std::size_t ElementSize( ) const { return m_elementSize; }

	template <typename T>
	T* Lock( )
	{
		return static_cast<T*>( LockImple( ) );
	}

	void Unlock( );

	operator aga::Buffer*( )
	{
		return m_buffer.Get( );
	}

protected:
	RefHandle<aga::Buffer> m_buffer;
	std::size_t m_elementSize = 0;
	std::size_t m_numElement = 0;
	char* m_data = 0;

private:
	void* LockImple( );
};