#pragma once

#include "../common.h"
#include "../../Engine/ScriptKeyHandler.h"

#include <memory>

struct D3D11_SAMPLER_DESC;
struct ID3D11Device;
struct ID3D11DeviceContext;
class IRenderState;
class KeyValueGroup;

class CD3D11SamplerStateFactory : public CScriptKeyHandler<CD3D11SamplerStateFactory>
{
public:
	void LoadDesc( );
	IRenderState* GetSamplerState( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& stateName );
	void AddSamplerDesc( const String& descName, const D3D11_SAMPLER_DESC& newDesc );

	CD3D11SamplerStateFactory( );
private:
	void LoadSamplerDesc( KeyValueGroup* pKeyValues );

	std::map<String, std::unique_ptr<IRenderState>> m_samplerState;
	std::map<String, D3D11_SAMPLER_DESC> m_samplerStateDesc;
};
