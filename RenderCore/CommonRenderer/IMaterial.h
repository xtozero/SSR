#pragma once

#include <array>
#include <memory>

class IBuffer;
class IRenderer;
class IRenderResource;
class ISurface;

enum SHADER_TYPE
{
	NONE = -1,
	VS,
	HS,
	DS,
	GS,
	PS,
	CS,
	MAX_SHADER
};

enum MAT_CONSTANT_BUFFER
{
	SURFACE = 0,
	TOTAL_COUNT
};

using MatConstantBuffers = std::array<IBuffer*, MAT_CONSTANT_BUFFER::TOTAL_COUNT>;

class IMaterial
{
public:
	virtual void Init( IRenderer& renderer ) = 0;
	virtual void SetShader( ) = 0;
	virtual void SetTexture( UINT shaderType, UINT slot, const IRenderResource* pTexture ) = 0;
	virtual void SetSurface( UINT shaderType, UINT slot, const ISurface* pSurface ) = 0;

	virtual void SetConstantBuffers( MatConstantBuffers* pConstantBuffers ) = 0;

	virtual ~IMaterial( ) = default;

protected:
	IMaterial( ) = default;
};