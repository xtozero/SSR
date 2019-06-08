#pragma once

#include "Render/IRenderResource.h"

#include <array>
#include <d3d11.h>
#include <wrl/client.h>

struct CD3D11DepthStencilDesc : public CD3D11_DEPTH_STENCIL_DESC
{
public:
	using CD3D11_DEPTH_STENCIL_DESC::CD3D11_DEPTH_STENCIL_DESC;
	unsigned int StencilRef = 0;
};

class CD3D11DepthStencilState: public IRenderResource
{
public:
	ID3D11DepthStencilState* Get( ) const { return m_pDepthStencilState.Get( ); }

	unsigned int GetStencilRef() const { return m_stencilRef; }

	bool Create( ID3D11Device& device, const CD3D11DepthStencilDesc& dsDesc )
	{
		m_stencilRef = dsDesc.StencilRef;
		return SUCCEEDED( device.CreateDepthStencilState( &dsDesc, &m_pDepthStencilState ) );
	}

	void SetStencilRef( unsigned int stencilRef ) { m_stencilRef = stencilRef; }

	CD3D11DepthStencilState( ) = default;
	CD3D11DepthStencilState( const CD3D11DepthStencilState& ) = delete;
	CD3D11DepthStencilState( CD3D11DepthStencilState&& ) = default;
	CD3D11DepthStencilState& operator=( const CD3D11DepthStencilState& ) = delete;
	CD3D11DepthStencilState& operator=( CD3D11DepthStencilState&& ) = default;
	~CD3D11DepthStencilState( ) = default;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
	unsigned int m_stencilRef = 0;
};

class CD3D11RasterizerState : public IRenderResource
{
public:
	ID3D11RasterizerState* Get( ) const { return m_pRasterizerState.Get( ); }

	bool Create( ID3D11Device& device, const D3D11_RASTERIZER_DESC& rsDesc )
	{
		return SUCCEEDED( device.CreateRasterizerState( &rsDesc, &m_pRasterizerState ) );
	}

	CD3D11RasterizerState( ) = default;
	CD3D11RasterizerState( const CD3D11RasterizerState& ) = delete;
	CD3D11RasterizerState( CD3D11RasterizerState&& ) = default;
	CD3D11RasterizerState& operator=( const CD3D11RasterizerState& ) = delete;
	CD3D11RasterizerState& operator=( CD3D11RasterizerState&& ) = default;
	~CD3D11RasterizerState( ) = default;

private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerState;
};

class CD3D11SamplerState : public IRenderResource
{
public:
	ID3D11SamplerState* Get( ) const { return m_pSamplerState.Get( ); }

	bool Create( ID3D11Device& device, const D3D11_SAMPLER_DESC& samplerDesc )
	{
		return SUCCEEDED( device.CreateSamplerState( &samplerDesc, &m_pSamplerState ) );
	}

	CD3D11SamplerState( ) = default;
	CD3D11SamplerState( const CD3D11SamplerState& ) = delete;
	CD3D11SamplerState( CD3D11SamplerState&& ) = default;
	CD3D11SamplerState& operator=( const CD3D11SamplerState& ) = delete;
	CD3D11SamplerState& operator=( CD3D11SamplerState&& ) = default;
	~CD3D11SamplerState( ) = default;

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;
};

struct CD3D_BLEND_DESC
{
	CD3D11_BLEND_DESC m_desc = CD3D11_BLEND_DESC( CD3D11_DEFAULT( ) );
	std::array<float, 4> m_blendFactor = { 0, };
	unsigned int m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
};

class CD3D11BlendState : public IRenderResource
{
public:
	ID3D11BlendState* Get( ) const { return m_pBlendState.Get( ); }

	const float* GetBlendFactor( ) const { return m_blendFactor.data( ); }

	unsigned int GetSamplerMask( ) const { return m_sampleMask; }

	bool Create( ID3D11Device& device, const CD3D_BLEND_DESC& blendDesc )
	{
		return SUCCEEDED( device.CreateBlendState( &blendDesc.m_desc, &m_pBlendState ) );
	}

	void SetBlendFactor( const std::array<float, 4>& blendFactor ) noexcept
	{
		m_blendFactor = blendFactor;
	}

	void SetSampleMask( unsigned int sampleMask ) noexcept { m_sampleMask = sampleMask; }

	CD3D11BlendState( ) = default;
	CD3D11BlendState( const CD3D11BlendState& ) = delete;
	CD3D11BlendState( CD3D11BlendState&& ) = default;
	CD3D11BlendState& operator=( const CD3D11BlendState& ) = delete;
	CD3D11BlendState& operator=( CD3D11BlendState&& ) = default;
	~CD3D11BlendState( ) = default;

private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendState;
	std::array<float, 4> m_blendFactor = { 0, };
	unsigned int m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
};