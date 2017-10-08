#pragma once

#include "../common.h"
#include "../../Engine/ScriptKeyHandler.h"

#include <memory>

struct D3D11_RASTERIZER_DESC;
struct ID3D11Device;
struct ID3D11DeviceContext;
class IRenderState;
class KeyValueGroup;

class CD3D11RasterizerStateFactory : public CScriptKeyHandler<CD3D11RasterizerStateFactory>
{
public:
	void LoadDesc( );
	IRenderState* GetRasterizerState( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& stateName );
	void AddRasterizerDesc( const String& descName, const D3D11_RASTERIZER_DESC& newDesc );

	CD3D11RasterizerStateFactory( );
private:
	void LoadRasterizerDesc( KeyValueGroup* pKeyValues );

	std::map<String, std::unique_ptr<IRenderState>> m_rasterizerState;
	std::map<String, D3D11_RASTERIZER_DESC> m_rasterizerStateDesc;
};
