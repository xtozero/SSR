#pragma once

#include "GraphicsApiResource.h"
#include "Math/CXMFloat.h"

#include <vector>

template <typename T>
class VertexBuffer
{
public:
	void Initialize( std::vector<T>&& rawData )
	{
		m_rawData = std::move( rawData );
	}

private:
	RE_HANDLE m_resource;
	std::vector<T> m_rawData;
};