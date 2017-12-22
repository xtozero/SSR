#pragma once

#include <D3D11.h>
#include <DXGI.h>

struct ID3D11BlendState;
struct ID3D11Buffer;
struct ID3D11ClassInstance;
struct ID3D11ComputeShader;
struct ID3D11DepthStencilState;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11RasterizerState;
struct ID3D11Resource;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11UnorderedAccessView;

class CD3D11RenderStateManager
{
public:
	void SetInputLayout( ID3D11InputLayout* pInputLayout );
	void SetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY topology );
	void SetVertexBuffer( UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets );
	void SetIndexBuffer( ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT offset );

	void SetVertexShader( ID3D11VertexShader* pVertexSahder, ID3D11ClassInstance* const* ppClassInstance, UINT NumClassInstances );
	void SetPixelShader( ID3D11PixelShader* pPixelShader, ID3D11ClassInstance* const* ppClassInstance, UINT NumClassInstances );
	void SetComputeShader( ID3D11ComputeShader* pComputeShader, ID3D11ClassInstance* const* ppClassInstance, UINT NumClassInstances );

	void SetVsShaderResource( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews );
	void SetPsShaderResource( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews );
	void SetCsShaderResource( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews );

	void SetCsUnorderedAccess( UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts );

	void SetVsConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers );
	void SetPsConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers );
	void SetCsConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers );

	void SetVsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers );
	void SetHsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers );
	void SetDsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers );
	void SetGsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers );
	void SetPsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers );
	void SetCsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers );

	void SetRasterizerState( ID3D11RasterizerState *pRasterizerState );

	void SetBlendState( ID3D11BlendState* pBlendState, const float BlendFactor[4], UINT SampleMask );
	void SetDepthStencilState( ID3D11DepthStencilState* pDepthStencilState, UINT StencilRef );

	HRESULT Map( ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource );
	void Unmap( ID3D11Resource* pResource, UINT Subresource );

	void Dispatch( UINT x, UINT y, UINT z );

	CD3D11RenderStateManager( ID3D11DeviceContext& deviceContext ) : m_deviceContext( deviceContext ) {}

private:
	ID3D11DeviceContext& m_deviceContext;

	ID3D11InputLayout* m_pCurInputLayout = nullptr;
	ID3D11VertexShader* m_pCurVS = nullptr;
	ID3D11PixelShader* m_pCurPS = nullptr;
	ID3D11ComputeShader* m_pCurCS = nullptr;

	ID3D11ShaderResourceView* m_pCurPSSRV[128] = { nullptr };

	ID3D11Buffer* m_pCurVsBuffer[15] = { nullptr };
	ID3D11Buffer* m_pCurPsBuffer[15] = { nullptr };
	ID3D11Buffer* m_pCurCsBuffer[15] = { nullptr };

	ID3D11SamplerState* m_pCurVsSampler[16] = { nullptr };
	ID3D11SamplerState* m_pCurHsSampler[16] = { nullptr };
	ID3D11SamplerState* m_pCurDsSampler[16] = { nullptr };
	ID3D11SamplerState* m_pCurGsSampler[16] = { nullptr };
	ID3D11SamplerState* m_pCurPsSampler[16] = { nullptr };
	ID3D11SamplerState* m_pCurCsSampler[16] = { nullptr };

	ID3D11RasterizerState* m_pCurRasterizerState = nullptr;
	ID3D11BlendState* m_pCurBlendState = nullptr;

	ID3D11DepthStencilState* m_pCurDepthStencilState = nullptr;
	UINT m_curStencilRef = 0;

	D3D11_PRIMITIVE_TOPOLOGY m_curTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
};

