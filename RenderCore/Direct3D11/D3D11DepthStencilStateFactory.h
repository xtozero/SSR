#pragma once

#include "../common.h"
#include "../../shared/Util.h"

#include <map>
#include <memory>

struct D3D11_DEPTH_STENCIL_DESC;
struct ID3D11Device;
class CD3D11DepthStencilState;
class CD3D11RenderStateManager;
class KeyValue;

class CD3D11DepthStencilStateFactory
{
public:
	void LoadDesc( );
	Owner<CD3D11DepthStencilState*> GetDepthStencilState( ID3D11Device& device, const String& stateName );
	void AddDepthStencilDesc( const String & descName, const D3D11_DEPTH_STENCIL_DESC & newDesc );

private:
	void LoadDepthStencilDesc( const KeyValue& keyValue );

	std::map<String, D3D11_DEPTH_STENCIL_DESC> m_depthStencilDesc;
};