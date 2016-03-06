#pragma once

#include "IMaterial.h"
#include "ISampler.h"

#include <D3D11.h>
#include <wrl/client.h>

class IShader;

class Material : public IMaterial
{
protected:
	std::shared_ptr<IShader> m_pShaders[SHADER_TYPE::MAX_SHADER];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRenderState;
	std::shared_ptr<ISampler> m_pSamplerState[SHADER_TYPE::MAX_SHADER];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;

public:
	virtual void Init( ) override;
	virtual void SetShader( ID3D11DeviceContext* pDeviceContext ) override;
	virtual void SetTexture( ID3D11DeviceContext* pDeviceContext, UINT shaderType, UINT slot, std::shared_ptr<ITexture> pTexture ) override;

	virtual void Draw( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT vertexOffset = 0 ) override;
	virtual void DrawIndexed( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT indexOffset = 0, const UINT vertexOffset = 0 ) override;
	void DrawInstanced( ID3D11DeviceContext* pDeviceContext, const UINT vertexCount, const UINT instanceCount, const UINT vertexOffset = 0, const UINT instanceOffset = 0 );
	void DrawInstancedInstanced( ID3D11DeviceContext* pDeviceContext, const UINT indexCount, const UINT instanceCount, const UINT indexOffset = 0, const UINT vertexOffset = 0, const UINT instanceOffset = 0 );
	void DrawAuto( ID3D11DeviceContext* pDeviceContext );

	virtual void SetPrimitiveTopology( ID3D11DeviceContext* pDeviceContext, D3D_PRIMITIVE_TOPOLOGY primtopology ) override;

	Material( );
};