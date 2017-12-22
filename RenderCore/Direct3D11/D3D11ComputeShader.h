#pragma once

#include "../CommonRenderer/IShader.h"

#include <wrl/client.h>

class CD3D11RenderStateManager;

struct ID3D11ComputeShader;
struct ID3D11Device;
struct ID3D11UnorderedAccessView;

class D3D11ComputeShader : public IComputeShader
{
public:
	virtual void SetShader( ) override;
	virtual void SetShaderResource( UINT slot, const IRenderResource* pResource ) override;
	virtual void SetRandomAccessResource( UINT slot, const IRenderResource* pResource ) override;
	virtual void Dispatch( UINT x, UINT y, UINT z = 1.f ) override;

	bool CreateShader( ID3D11Device& device, const void* byteCodePtr, const size_t byteCodeSize );

	D3D11ComputeShader( CD3D11RenderStateManager& renderStateManager ) : m_renderStateManager( renderStateManager ) {}

private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_pComputeShader;
	CD3D11RenderStateManager& m_renderStateManager;
};

