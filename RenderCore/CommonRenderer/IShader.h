#pragma once

#include "../common.h"

class IBuffer;
class IRenderResource;

class IShader
{
public:
	virtual void SetShader ( ) = 0;
	virtual void SetShaderResource( UINT slot, const IRenderResource* pResource ) = 0;
	virtual void SetConstantBuffer( UINT slot, const IBuffer* pBuffer ) = 0;

	virtual ~IShader( ) = default;
};