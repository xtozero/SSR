#pragma once

#include <D3Dcommon.h>
#include <memory>

class IShaderResource;
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
	virtual void SetTexture( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, std::shared_ptr<IShaderResource> pTexture ) = 0;
	virtual void Draw( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT vertexOffset = 0 ) = 0;
	virtual void DrawIndexed( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT indexOffset = 0, const UINT vertexOffset = 0 ) = 0;

	virtual void SetPrimitiveTopology( ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY primtopology ) = 0;

	virtual ~IMaterial( ) = default;

protected:
	IMaterial( ) = default;
};