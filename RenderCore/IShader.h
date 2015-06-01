#pragma once

#include "common.h"

class IShaderTrait
{
protected:
	IShaderTrait () {}

public:
	virtual ~IShaderTrait () {}
};

class IShader
{
public:
	virtual bool CreateShader ( const IShaderTrait& trait ) = 0;
	virtual void SetShader ( const IShaderTrait& trait ) = 0;

protected:
	ID3D10Blob* m_shaderBlob;

	IShader () {}

public:
	virtual ~IShader ()
	{
		SAFE_RELEASE ( m_shaderBlob );
	}
};