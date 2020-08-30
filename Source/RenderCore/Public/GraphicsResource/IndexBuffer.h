#pragma once

#include "GraphicsApiResource.h"

#include <vector>

class IndexBuffer
{
public:
	void Initialize( const std::vector<std::size_t>& rawData );

	~IndexBuffer( );

private:
	RE_HANDLE m_resource;
	void* m_rawData = nullptr;
	unsigned char m_stride = 0;
};