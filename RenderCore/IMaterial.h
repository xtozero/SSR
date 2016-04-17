#pragma once

#include "MaterialCommon.h"

#include <D3Dcommon.h>
#include <memory>

class IShaderResource;
class ISurface;
struct ID3D11DeviceContext;

enum SHADER_TYPE
{
	VS = 0,
	HS,
	DS,
	GS,
	PS,
	CS,
	MAX_SHADER
};

class IMaterial
{
public:
	virtual void Init( ) = 0;
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) = 0;
	virtual void SetTexture( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, const IShaderResource* pTexture ) = 0;
	virtual void SetSurface( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, const ISurface* pSurface ) = 0;
	
	virtual void Draw( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT vertexOffset = 0 ) = 0;
	virtual void DrawIndexed( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT indexOffset = 0, const UINT vertexOffset = 0 ) = 0;

	virtual void SetPrimitiveTopology( ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY primtopology ) = 0;
	virtual void SetConstantBuffers( MatConstantBuffers* pConstantBuffers ) = 0;

	virtual ~IMaterial( ) = default;

protected:
	IMaterial( ) = default;
};