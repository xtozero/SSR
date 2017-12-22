#pragma once

#include "../common.h"

#include <map>
#include <memory>

struct D3D11_SAMPLER_DESC;
struct ID3D11Device;
class CD3D11RenderStateManager;
class IRenderState;
class KeyValue;

class CD3D11SamplerStateFactory
{
public:
	void LoadDesc( );
	IRenderState* GetSamplerState( ID3D11Device& device, CD3D11RenderStateManager& renderStateManager, const String& stateName );
	void AddSamplerDesc( const String& descName, const D3D11_SAMPLER_DESC& newDesc );

private:
	void LoadSamplerDesc( const KeyValue& keyValue );

	std::map<String, std::unique_ptr<IRenderState>> m_samplerState;
	std::map<String, D3D11_SAMPLER_DESC> m_samplerStateDesc;
};
