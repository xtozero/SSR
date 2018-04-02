#pragma once

#include "D3D11RenderState.h"

#include "common.h"
#include "Util.h"

#include <map>
#include <memory>

class KeyValue;

class CD3D11SamplerStateFactory
{
public:
	void LoadDesc( );
	Owner<CD3D11SamplerState*> GetSamplerState( ID3D11Device& device, const String& stateName );

private:
	void LoadSamplerDesc( const KeyValue& keyValue );

	std::map<String, D3D11_SAMPLER_DESC> m_samplerStateDesc;
};

class CD3D11RasterizerStateFactory
{
public:
	void LoadDesc( );
	Owner<CD3D11RasterizerState*> GetRasterizerState( ID3D11Device& device, const String& stateName );

private:
	void LoadRasterizerDesc( const KeyValue& keyValue );

	std::map<String, D3D11_RASTERIZER_DESC> m_rasterizerStateDesc;
};

class CD3D11BlendStateFactory
{
public:
	void LoadDesc( );
	Owner<CD3D11BlendState*> GetBlendState( ID3D11Device& device, const String& stateName );

private:
	void LoadRasterizerDesc( const KeyValue& keyValue );

	std::map<String, CD3D_BLEND_DESC> m_blendStateDesc;
};

class CD3D11DepthStencilStateFactory
{
public:
	void LoadDesc( );
	Owner<CD3D11DepthStencilState*> GetDepthStencilState( ID3D11Device& device, const String& stateName );
	void AddDepthStencilDesc( const String & descName, const CD3D11DepthStencilDesc& newDesc );

private:
	void LoadDepthStencilDesc( const KeyValue& keyValue );

	std::map<String, CD3D11DepthStencilDesc> m_depthStencilDesc;
};