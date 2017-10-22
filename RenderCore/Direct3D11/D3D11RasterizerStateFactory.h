#pragma once

#include "../common.h"

#include <map>
#include <memory>

struct D3D11_RASTERIZER_DESC;
struct ID3D11Device;
struct ID3D11DeviceContext;
class IRenderState;
class KeyValue;

class CD3D11RasterizerStateFactory
{
public:
	void LoadDesc( );
	IRenderState* GetRasterizerState( ID3D11Device& device, ID3D11DeviceContext& deviceContext, const String& stateName );
	void AddRasterizerDesc( const String& descName, const D3D11_RASTERIZER_DESC& newDesc );

private:
	void LoadRasterizerDesc( const KeyValue& keyValue );

	std::map<String, std::unique_ptr<IRenderState>> m_rasterizerState;
	std::map<String, D3D11_RASTERIZER_DESC> m_rasterizerStateDesc;
};
