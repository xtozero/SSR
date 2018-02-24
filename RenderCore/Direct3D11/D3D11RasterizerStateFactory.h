#pragma once

#include "../common.h"
#include "../../shared/Util.h"

#include <map>
#include <memory>

struct D3D11_RASTERIZER_DESC;
struct ID3D11Device;
class CD3D11RasterizerState;
class KeyValue;

class CD3D11RasterizerStateFactory
{
public:
	void LoadDesc( );
	Owner<CD3D11RasterizerState*> GetRasterizerState( ID3D11Device& device, const String& stateName );

private:
	void LoadRasterizerDesc( const KeyValue& keyValue );

	std::map<String, D3D11_RASTERIZER_DESC> m_rasterizerStateDesc;
};
