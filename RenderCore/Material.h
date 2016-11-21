#pragma once

#include "IMaterial.h"

#include <wrl/client.h>

class IShader;
class IBuffer;
class IRenderState;

class Material : public IMaterial
{
protected:
	std::shared_ptr<IShader> m_pShaders[SHADER_TYPE::MAX_SHADER];
	std::shared_ptr<IRenderState> m_pRasterizerState;
	std::shared_ptr<IRenderState> m_pSamplerState[SHADER_TYPE::MAX_SHADER];
	std::shared_ptr<IRenderState> m_pDepthStencilState;
	std::shared_ptr<IRenderState> m_pBlendState;
	MatConstantBuffers* m_pConstantBuffers;

public:
	virtual void Init( IRenderer& renderer ) override;
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) override;
	virtual void SetTexture( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, const IShaderResource* pTexture ) override;
	virtual void SetSurface( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, const ISurface* pSurface ) override;

	virtual void Draw( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT vertexOffset = 0 ) override;
	virtual void DrawIndexed( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT indexOffset = 0, const UINT vertexOffset = 0 ) override;
	void DrawInstanced( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT instanceCount, const UINT vertexOffset = 0, const UINT instanceOffset = 0 );
	void DrawInstancedInstanced( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT instanceCount, const UINT indexOffset = 0, const UINT vertexOffset = 0, const UINT instanceOffset = 0 );
	void DrawAuto( ID3D11DeviceContext* pDeviceContext );

	virtual void SetPrimitiveTopology( ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY primtopology ) override;
	
	virtual void SetConstantBuffers( MatConstantBuffers* pConstantBuffers ) override { m_pConstantBuffers = pConstantBuffers; }

	Material( );
};