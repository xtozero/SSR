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
	void OnDeviceRestore( ID3D11DeviceContext* pDeviceContext ) { m_pDeviceContext = pDeviceContext; }

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

	void SetRenderTargets( UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView );

	CD3D11RenderStateManager( ) = default;
	~CD3D11RenderStateManager( ) = default;
	CD3D11RenderStateManager( const CD3D11RenderStateManager& ) = delete;
	CD3D11RenderStateManager( CD3D11RenderStateManager&& ) = delete;
	CD3D11RenderStateManager& operator=( const CD3D11RenderStateManager& ) = delete;
	CD3D11RenderStateManager& operator=( CD3D11RenderStateManager&& ) = delete;

private:
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

	ID3D11InputLayout* m_pCurInputLayout = nullptr;
	ID3D11VertexShader* m_pCurVS = nullptr;
	ID3D11PixelShader* m_pCurPS = nullptr;
	ID3D11ComputeShader* m_pCurCS = nullptr;

	ID3D11ShaderResourceView* m_pCurVsSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	ID3D11ShaderResourceView* m_pCurPsSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	ID3D11ShaderResourceView* m_pCurCsSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };

	ID3D11Buffer* m_pCurVsBuffer[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = { nullptr };
	ID3D11Buffer* m_pCurPsBuffer[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = { nullptr };
	ID3D11Buffer* m_pCurCsBuffer[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = { nullptr };

	ID3D11SamplerState* m_pCurVsSampler[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { nullptr };
	ID3D11SamplerState* m_pCurHsSampler[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { nullptr };
	ID3D11SamplerState* m_pCurDsSampler[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { nullptr };
	ID3D11SamplerState* m_pCurGsSampler[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { nullptr };
	ID3D11SamplerState* m_pCurPsSampler[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { nullptr };
	ID3D11SamplerState* m_pCurCsSampler[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { nullptr };

	ID3D11RasterizerState* m_pCurRasterizerState = nullptr;
	ID3D11BlendState* m_pCurBlendState = nullptr;

	ID3D11DepthStencilState* m_pCurDepthStencilState = nullptr;
	UINT m_curStencilRef = 0;

	D3D11_PRIMITIVE_TOPOLOGY m_curTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
};

