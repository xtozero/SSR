#pragma once

#include "../common.h"
#include "../../Engine/ScriptKeyHandler.h"

#include <memory>

struct D3D11_DEPTH_STENCIL_DESC;
struct ID3D11Device;
struct ID3D11DeviceContext;
class IRenderState;
class KeyValueGroup;

class CD3D11DepthStencilStateFactory : public CScriptKeyHandler<CD3D11DepthStencilStateFactory>
{
public:
	void LoadDesc( );
	IRenderState* GetDepthStencilState( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& stateName );
	void AddDepthStencilDesc( const String & descName, const D3D11_DEPTH_STENCIL_DESC & newDesc );

	CD3D11DepthStencilStateFactory( );
private:
	void LoadDepthStencilDesc( const KeyValueGroup* pKeyValues );

	std::map<String, std::unique_ptr<IRenderState>> m_depthStencilState;
	std::map<String, D3D11_DEPTH_STENCIL_DESC> m_depthStencilDesc;
};