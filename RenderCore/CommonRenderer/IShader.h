#pragma once

#include "../common.h"

class IBuffer;
class IRenderResource;

class IShader
{
public:
	virtual void SetShader ( ) = 0;
	virtual void SetShaderResource( UINT slot, const IRenderResource* pResource ) = 0;

	virtual ~IShader( ) = default;
};

class IComputeShader
{
public:
	virtual void SetShader( ) = 0;
	virtual void SetShaderResource( UINT slot, const IRenderResource* pResource ) = 0;
	virtual void SetRandomAccessResource( UINT slot, const IRenderResource* pResource ) = 0;
	virtual void Dispatch( UINT x, UINT y, UINT z = 1 ) = 0;

	virtual ~IComputeShader( ) = default;
};