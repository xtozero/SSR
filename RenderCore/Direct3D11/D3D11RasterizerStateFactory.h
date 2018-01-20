#pragma once

#include "../common.h"

#include <map>
#include <memory>

struct D3D11_RASTERIZER_DESC;
struct ID3D11Device;
class CD3D11RenderStateManager;
class IRenderState;
class KeyValue;

class CD3D11RasterizerStateFactory
{
public:
	void OnDeviceLost( );
	void LoadDesc( );
	IRenderState* GetRasterizerState( ID3D11Device& device, CD3D11RenderStateManager& renderStateManager, const String& stateName );
	void AddRasterizerDesc( const String& descName, const D3D11_RASTERIZER_DESC& newDesc );

private:
	void LoadRasterizerDesc( const KeyValue& keyValue );

	std::map<String, std::unique_ptr<IRenderState>> m_rasterizerState;
	std::map<String, D3D11_RASTERIZER_DESC> m_rasterizerStateDesc;
};
