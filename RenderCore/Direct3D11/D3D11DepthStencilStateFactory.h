#pragma once

#include "../common.h"

#include <map>
#include <memory>

struct D3D11_DEPTH_STENCIL_DESC;
struct ID3D11Device;
class CD3D11RenderStateManager;
class IRenderState;
class KeyValue;

class CD3D11DepthStencilStateFactory
{
public:
	void LoadDesc( );
	IRenderState* GetDepthStencilState( ID3D11Device& device, CD3D11RenderStateManager& renderStateManager, const String& stateName );
	void AddDepthStencilDesc( const String & descName, const D3D11_DEPTH_STENCIL_DESC & newDesc );

private:
	void LoadDepthStencilDesc( const KeyValue& keyValue );

	std::map<String, std::unique_ptr<IRenderState>> m_depthStencilState;
	std::map<String, D3D11_DEPTH_STENCIL_DESC> m_depthStencilDesc;
};