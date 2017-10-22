#pragma once

#include "../common.h"

#include <array>
#include <d3d11.h>
#include <map>
#include <memory>

struct D3D11_BLEND_DESC;
struct ID3D11Device;
struct ID3D11DeviceContext;
class IRenderState;
class KeyValue;

struct CD3D_BLEND_DESC
{
	CD3D11_BLEND_DESC m_desc = CD3D11_BLEND_DESC( CD3D11_DEFAULT( ) );
	std::array<float, 4> m_blendFactor = { 0, };
	unsigned int m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
};

class CD3D11BlendStateFactory
{
public:
	void LoadDesc( );
	IRenderState* GetBlendState( ID3D11Device& device, ID3D11DeviceContext& deviceContext, const String& stateName );
	void AddBlendDesc( const String& descName, const CD3D_BLEND_DESC& newDesc );

private:
	void LoadRasterizerDesc( const KeyValue& keyValue );

	std::map<String, std::unique_ptr<IRenderState>> m_blendState;
	std::map<String, CD3D_BLEND_DESC> m_blendStateDesc;
};
