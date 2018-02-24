#pragma once

#include "../common.h"
#include "../../shared/Util.h"

#include <map>
#include <memory>

struct D3D11_SAMPLER_DESC;
struct ID3D11Device;
class CD3D11RenderStateManager;
class CD3D11SamplerState;
class KeyValue;

class CD3D11SamplerStateFactory
{
public:
	void LoadDesc( );
	Owner<CD3D11SamplerState*> GetSamplerState( ID3D11Device& device, const String& stateName );

private:
	void LoadSamplerDesc( const KeyValue& keyValue );

	std::map<String, D3D11_SAMPLER_DESC> m_samplerStateDesc;
};
