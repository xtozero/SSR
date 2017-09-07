#pragma once

#include "common.h"
#include "../Engine/ScriptKeyHandler.h"

#include <memory>

struct D3D11_SAMPLER_DESC;
struct ID3D11Device;
class IRenderState;

class ISamplerStateFactory : public CScriptKeyHandler<ISamplerStateFactory>
{
public:
	virtual void LoadDesc( ) = 0;
	virtual IRenderState* GetSamplerState( ID3D11Device* pDevice, const String& stateName ) = 0;
	virtual void AddSamplerDesc( const String& descName, const D3D11_SAMPLER_DESC& newDesc ) = 0;

	virtual ~ISamplerStateFactory( ) = default;
};

std::unique_ptr<ISamplerStateFactory> CreateSamplerStateFactory( );