#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"

#include <cstddef>

class UploadBuffer
{
public:
	void Resize( std::size_t numElement, const void* initData );
	std::size_t ElementSize( ) const { return m_elementSize; }

	template <typename T>
	T* Lock( )
	{
		return static_cast<T*>( LockImple( ) );
	}

	void Unlock( );

	aga::Buffer* Resource( );
	const aga::Buffer* Resource( ) const;

	UploadBuffer( std::size_t elementSize, std::size_t numElement, const void* initData );

	UploadBuffer( ) = default;
	~UploadBuffer( ) = default;
	UploadBuffer( const UploadBuffer& ) = default;
	UploadBuffer& operator=( const UploadBuffer& ) = default;
	UploadBuffer( UploadBuffer&& ) = default;
	UploadBuffer& operator=( UploadBuffer&& ) = default;

protected:
	void InitResource( const void* initData );

	RefHandle<aga::Buffer> m_buffer;
	std::size_t m_elementSize = 0;
	std::size_t m_numElement = 0;

private:
	void* LockImple( );
};