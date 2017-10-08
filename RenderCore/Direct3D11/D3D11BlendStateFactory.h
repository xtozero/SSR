#pragma once

#include "../common.h"
#include "../../Engine/ScriptKeyHandler.h"

#include <memory>

struct CD3D_BLEND_DESC;
struct D3D11_BLEND_DESC;
struct ID3D11Device;
struct ID3D11DeviceContext;
class IRenderState;
class KeyValueGroup;

class CD3D11BlendStateFactory : public CScriptKeyHandler<CD3D11BlendStateFactory>
{
public:
	void LoadDesc( );
	IRenderState* GetBlendState( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& stateName );
	void AddBlendDesc( const String& descName, const CD3D_BLEND_DESC& newDesc );

	CD3D11BlendStateFactory( );
private:
	void LoadRasterizerDesc( const KeyValueGroup* pKeyValues );

	std::map<String, std::unique_ptr<IRenderState>> m_blendState;
	std::map<String, CD3D_BLEND_DESC> m_blendStateDesc;
};
